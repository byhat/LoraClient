#include "AppEngine.hpp"
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QSerialPortInfo>

#include <src/LoRaWorker.hpp>

AppEngine::AppEngine(QObject *parent)
: QObject(parent)
, m_baudRate(9600)
, m_isConnected(false)
{
    initializeLoRaWorker();
    updateAvailablePorts();
}

AppEngine::~AppEngine()
{
    if (m_loRaWorker) {
        m_loRaWorker->deleteLater();
    }
}

void AppEngine::initializeLoRaWorker()
{
    if (m_loRaWorker) return;

    m_loRaWorker = new LoRaWorker(this);
    connect(m_loRaWorker, &LoRaWorker::portOpened, this, [this](bool ok, const QString &error) {
        m_isConnected = ok;
        m_lastError = ok ? "" : error;
        emit isConnectedChanged();
        emit errorOccurred();
    });

    connect(m_loRaWorker, &LoRaWorker::packetSent, this, [this](bool success) {
        if (!success) {
            addMessage("Ошибка отправки пакета", "error");
        }

        m_sendBytes = m_sendTotal = 0;
        m_sendProgress = 0;

        emit sendProgressChanged();
    });

    connect(m_loRaWorker, &LoRaWorker::packetReceived, this, [this](const QByteArray &data) {
        QString receivedText = QString::fromUtf8(data);
        m_receiveBytes = m_receiveTotal = 0;
        m_receiveProgress = 0;

        emit receiveProgressChanged();

        addMessage(receivedText, "received");
    });

    connect(m_loRaWorker, &LoRaWorker::errorOccurred, this, [this](const QString &msg) {
        m_lastError = msg;
        emit errorOccurred();
        addMessage("Ошибка: " + msg, "error");
    });

    connect(m_loRaWorker, &LoRaWorker::packetSendProgress, this, [this](int sentBytes, int totalBytes) {
        m_sendBytes = sentBytes;
        m_sendTotal = totalBytes;
        m_sendProgress = totalBytes > 0 ? static_cast<int>((sentBytes * 100.0) / totalBytes) : 0;

        emit sendProgressChanged();
    });

    connect(m_loRaWorker, &LoRaWorker::packetReceiveProgress, this, [this](int receivedBytes, int totalBytes) {
        m_receiveBytes = receivedBytes;
        m_receiveTotal = totalBytes;
        m_receiveProgress = totalBytes > 0 ? static_cast<int>((receivedBytes * 100.0) / totalBytes) : 0;

        emit receiveProgressChanged();
    });
}

QString AppEngine::portName() const
{
    return m_portName;
}

void AppEngine::setPortName(const QString &portName)
{
    if (m_portName == portName) return;
    m_portName = portName;
    emit portNameChanged();
}

qint32 AppEngine::baudRate() const
{
    return m_baudRate;
}

void AppEngine::setBaudRate(qint32 baudRate)
{
    if (m_baudRate == baudRate) return;
    m_baudRate = baudRate;
    emit baudRateChanged();
}

bool AppEngine::isConnected() const
{
    return m_isConnected;
}

QString AppEngine::lastError() const
{
    return m_lastError;
}

QVariantList AppEngine::messages() const
{
    return m_messages;
}

QVariantList AppEngine::availablePorts() const
{
    return m_availablePorts;
}

int AppEngine::sendProgress() const
{
    return m_sendProgress;
}
int AppEngine::receiveProgress() const
{
    return m_receiveProgress;
}

QString AppEngine::sendProgressText() const {
    return QString("%1 из %2 байт").arg(m_sendBytes).arg(m_sendTotal);
}

QString AppEngine::receiveProgressText() const {
    return QString("%1 из %2 байт").arg(m_receiveBytes).arg(m_receiveTotal);
}

void AppEngine::updateAvailablePorts()
{
    m_availablePorts.clear();
    
    const auto ports = QSerialPortInfo::availablePorts();
    for (const auto &port : ports) {
        m_availablePorts.append(port.portName());
    }
    
    emit availablePortsChanged();
}

void AppEngine::addMessage(const QString &text, const QString &type)
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

void AppEngine::openPort()
{
    if (!m_loRaWorker) return;
    m_loRaWorker->openPort(m_portName, m_baudRate);
}

void AppEngine::closePort()
{
    if (!m_loRaWorker) return;
    m_loRaWorker->closePort();
}

void AppEngine::sendTextMessage(const QString &text)
{
    if (!m_loRaWorker || text.isEmpty()) return;
    m_loRaWorker->sendPacket(text.toUtf8());
    addMessage(text, "sent");
}
