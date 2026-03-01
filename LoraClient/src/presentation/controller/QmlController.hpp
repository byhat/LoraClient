#pragma once
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QImage>
#include "QCrossPlatformSerialPortInfo.hpp"

#include "src/core/entities/MsgStructures.hpp"

// Forward declarations
class SaveImageUseCase;


class QmlController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString portName MEMBER m_portName NOTIFY portNameChanged)
    Q_PROPERTY(qint32 baudRate MEMBER m_baudRate NOTIFY baudRateChanged)
    Q_PROPERTY(bool isConnected MEMBER m_isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QStringList availablePorts MEMBER m_availablePorts NOTIFY availablePortsChanged)

    Q_PROPERTY(QVariantList messages MEMBER m_messages NOTIFY messagesChanged)

    Q_PROPERTY(int sendProgress READ sendProgress NOTIFY sendProgressChanged)
    Q_PROPERTY(int receiveProgress READ receiveProgress NOTIFY receiveProgressChanged)

    Q_PROPERTY(QString sendProgressText READ sendProgressText NOTIFY sendProgressChanged)
    Q_PROPERTY(QString receiveProgressText READ receiveProgressText NOTIFY receiveProgressChanged)

public:
    explicit QmlController(QObject *parent = nullptr);

public slots:
    // qml invoke slots
    void onSendText(QString msg);
    void onSendImage(QString path);
    void onSendFile(QString path);
    Q_INVOKABLE void saveImage(const QString &base64Data, const QString &timestamp);
    Q_INVOKABLE void saveImageToPath(const QString &base64Data, const QString &timestamp, const QString &filePath);

    void onOpenPort();
    void onClosePort();
    void onGetInterfacesList();
    void onUpdateInterfacesList(QStringList lst);

    int sendProgress() const;
    int receiveProgress() const;

    QString sendProgressText() const;
    QString receiveProgressText() const;

    // receive slots
    void portOpened(bool ok, const QString &error);
    void packetSent(bool success);
    void textReceived(const TextMsg &txt);
    void imageReceived(const ImageMsg &img);
    void errorOccurred(const QString &msg);
    void packetSendProgress(int sentBytes, int totalBytes);
    void packetReceiveProgress(int receivedBytes, int totalBytes);

    // SaveImageUseCase slots
    void imageSaved(const QString &filePath);
    void saveImageError(const QString &errorMessage);

signals:
    // qml data update signals
    void portNameChanged();
    void baudRateChanged();
    void isConnectedChanged();
    void errorOccurred();
    void availablePortsChanged();
    void messagesChanged();
    void sendProgressChanged();
    void receiveProgressChanged();

    // actions request signals from qml
    void openPort(QVariantHash settings);
    void setSettings(QVariantHash settings);
    void closePort();
    void getInterfacesList();

    void sendText(QString msg);
    void sendImage(QString path);
    void sendFile(QString path);

    // Save image signals
    void imageSavedSignal(QString filePath);


private:
    QString m_portName;
    qint32 m_baudRate;
    bool m_isConnected = 0;
    QString m_lastError;
    QStringList m_availablePorts;

    QVariantList m_messages;

    int m_sendProgress = 0;
    int m_receiveProgress = 0;

    int m_sendBytes = 0;
    int m_sendTotal = 0;
    int m_receiveBytes = 0;
    int m_receiveTotal = 0;

    void addSentMessage(const QString &text, const QString &type);
    void addSentImage(const QString &path, const QImage &img);
    void addReceivedImage(const QString &path, const QImage &img);
    void initializeLoRaWorker();

    SaveImageUseCase *m_saveImageUseCase;

public:
    void setSaveImageUseCase(SaveImageUseCase *usecase);
};
