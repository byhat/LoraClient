#include "QmlController.hpp"
#include "src/core/usecases/SaveImageUseCase.hpp"

#include <QUrl>
#include <QBuffer>
#include <QFileInfo>

QmlController::QmlController(QObject *parent)
    : QObject{parent}
    , m_saveImageUseCase(nullptr)
{
}

void QmlController::onSendText(QString msg)
{
    addSentMessage(msg, "sent");
    emit sendText(msg);
}

void QmlController::onSendImage(QString path) {
    QUrl url(path);
    if (!url.isValid() || !url.isLocalFile()) {
        addSentMessage("Ошибка: некорректный или не локальный путь", "error");
        return;
    }

    QString localPath = url.toLocalFile();

    QFile file(localPath);
    if (!file.exists()) {
        addSentMessage("Ошибка: файл не найден: " + localPath, "error");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        addSentMessage("Ошибка: не удалось открыть файл для чтения: " + localPath, "error");
        return;
    }

    QByteArray imageData = file.readAll();
    file.close();

    if (!imageData.isEmpty()) {
        QString header = imageData.left(16).toHex(' ');
        qDebug() << "Первые 16 байт (HEX):" << header;
    }

    QImage img;
    if (!img.loadFromData(imageData)) {
        addSentMessage("Ошибка: не удалось загрузить изображение. Размер:" + QString::number(imageData.size()) + " байт", "error");
        return;
    }

    if (img.isNull()) {
        addSentMessage("Ошибка: изображение пустое или повреждено", "error");
        return;
    }

    qDebug() << "Изображение успешно загружено:" << img.size() << "пикселей";

    emit sendImage(localPath);
    addSentImage(localPath, img);
}

void QmlController::onSendFile(QString path)
{
    addSentMessage(path, "sent");
    emit onSendFile(path);
}

void QmlController::onOpenPort()
{
    QVariantHash settings;
    settings["portName"] = m_portName;
    settings["baud"]     = m_baudRate;

    emit openPort(settings);
}

void QmlController::onClosePort()
{
    emit closePort();
}

void QmlController::onGetInterfacesList()
{
    emit getInterfacesList();
}

void QmlController::onUpdateInterfacesList(QStringList lst)
{
    m_availablePorts = lst;
    emit availablePortsChanged();
}

void QmlController::portOpened(bool ok, const QString &error)
{
    m_isConnected = ok;
    m_lastError = ok ? "" : error;
    emit isConnectedChanged();
    emit errorOccurred();
}

void QmlController::packetSent(bool success) {
    if (!success) {
        addSentMessage("Ошибка отправки пакета", "error");
    }

    m_sendBytes = m_sendTotal = 0;
    m_sendProgress = 0;

    emit sendProgressChanged();
}

void QmlController::textReceived(const TextMsg &txt) {
    m_receiveBytes = m_receiveTotal = 0;
    m_receiveProgress = 0;
    QVariantMap msg;
    msg["text"] = txt.text;
    msg["type"] = "received";
    msg["time"] = txt.time;

    m_messages.append(msg);

    if (m_messages.size() > 100) {
        m_messages.removeFirst();
    }

    emit messagesChanged();
    emit receiveProgressChanged();

}

void QmlController::imageReceived(const ImageMsg &img)
{
    m_receiveBytes = m_receiveTotal = 0;
    m_receiveProgress = 0;

    qDebug() << "imageReceived: img.isNull() =" << img.img.isNull() << ", size =" << img.img.size();

    if (img.img.isNull()) {
        qDebug() << "Warning: Received image is null!";
        emit errorOccurred("Получено пустое изображение");
        return;
    }

    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    img.img.save(&buffer, "PNG");

    QString base64 = ba.toBase64().toStdString().c_str();
    qDebug() << "imageReceived: base64 size =" << base64.size();

    QVariantMap msg;
    msg["type"] = "received";
    msg["time"] = img.time.toString("HH:mm:ss");
    msg["image"] = "data:image/png;base64," + base64;
    msg["text"] = "Изображение получено";

    m_messages.append(msg);

    if (m_messages.size() > 100) {
        m_messages.removeFirst();
    }

    emit messagesChanged();
    emit receiveProgressChanged();
}

void QmlController::errorOccurred(const QString &msg) {
    m_lastError = msg;
    emit errorOccurred();
    addSentMessage("Ошибка: " + msg, "error");
}

void QmlController::packetSendProgress(int sentBytes, int totalBytes) {
    m_sendBytes = sentBytes;
    m_sendTotal = totalBytes;
    m_sendProgress = totalBytes > 0 ? static_cast<int>((sentBytes * 100.0) / totalBytes) : 0;

    emit sendProgressChanged();
}

void QmlController::packetReceiveProgress(int receivedBytes, int totalBytes) {
    m_receiveBytes = receivedBytes;
    m_receiveTotal = totalBytes;
    m_receiveProgress = totalBytes > 0 ? static_cast<int>((receivedBytes * 100.0) / totalBytes) : 0;

    emit receiveProgressChanged();
}

int QmlController::sendProgress() const
{
    return m_sendProgress;
}

int QmlController::receiveProgress() const
{
    return m_receiveProgress;
}

QString QmlController::sendProgressText() const {
    return QString("%1 из %2 байт").arg(m_sendBytes).arg(m_sendTotal);
}

QString QmlController::receiveProgressText() const {
    return QString("%1 из %2 байт").arg(m_receiveBytes).arg(m_receiveTotal);
}

void QmlController::addSentMessage(const QString &text, const QString &type)
{
    QVariantMap msg;
    msg["text"] = text;
    msg["type"] = type;
    msg["time"] = QDateTime::currentDateTime().toString("HH:mm:ss");

    m_messages.append(msg);

    if (m_messages.size() > 100) {
        m_messages.removeFirst();
    }

    emit messagesChanged();
}

void QmlController::setSaveImageUseCase(SaveImageUseCase *usecase)
{
    m_saveImageUseCase = usecase;

    if (m_saveImageUseCase) {
        connect(m_saveImageUseCase, &SaveImageUseCase::imageSaved,
                this, &QmlController::imageSaved);
        connect(m_saveImageUseCase, &SaveImageUseCase::errorOccured,
                this, &QmlController::saveImageError);
    }
}

void QmlController::saveImage(const QString &base64Data, const QString &timestamp)
{
    if (m_saveImageUseCase) {
        m_saveImageUseCase->saveImage(base64Data, timestamp);
    } else {
        addSentMessage("Ошибка: SaveImageUseCase не инициализирован", "error");
    }
}

void QmlController::saveImageToPath(const QString &base64Data, const QString &timestamp, const QString &filePath)
{
    if (m_saveImageUseCase) {
        // Convert URL to local file path if needed
        QString localPath = filePath;
        QUrl url(filePath);
        if (url.isValid() && url.isLocalFile()) {
            localPath = url.toLocalFile();
        }
        m_saveImageUseCase->saveImageToPath(base64Data, timestamp, localPath);
    } else {
        addSentMessage("Ошибка: SaveImageUseCase не инициализирован", "error");
    }
}

void QmlController::imageSaved(const QString &filePath)
{
    addSentMessage("Изображение сохранено: " + filePath, "sent");
    emit imageSavedSignal(filePath);
}

void QmlController::saveImageError(const QString &errorMessage)
{
    addSentMessage("Ошибка сохранения: " + errorMessage, "error");
}

void QmlController::addSentImage(const QString &path, const QImage &img) {
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");

    QString base64 = ba.toBase64().toStdString().c_str();

    QVariantMap msg;
    msg["type"] = "sent";
    msg["time"] = QDateTime::currentDateTime().toString("HH:mm:ss");
    msg["image"] = "data:image/png;base64," + base64;
    msg["text"] = "Изображение: " + QFileInfo(path).fileName();

    m_messages.append(msg);

    if (m_messages.size() > 100) {
        m_messages.removeFirst();
    }

    emit messagesChanged();
}
