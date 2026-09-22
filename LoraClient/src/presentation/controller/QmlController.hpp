#pragma once
#include <QObject>
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QImage>
#include <memory>
#include "QCrossPlatformSerialPortInfo.hpp"

#include "src/core/entities/MsgStructures.hpp"
#include "src/domain/interfaces/IConnectionListener.hpp"
#include "src/domain/interfaces/IReceiveListener.hpp"
#include "src/domain/interfaces/ISaveImageListener.hpp"
#include "src/domain/interfaces/ISendListener.hpp"

// Forward declarations
class SaveImageUseCase;
class SendUseCase;
class ConnectionUseCase;


class QmlController : public QObject
                     , public IReceiveListener
                     , public ISendListener
                     , public IConnectionListener
                     , public ISaveImageListener {
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

    // IReceiveListener
    void onTextReceived(const TextMsg &txt) override;
    void onImageReceived(const ImageMsg &img) override;
    void onReceiveError(const QString &msg) override;

    // ISendListener
    void onSendError(const QString &msg) override;

    // IConnectionListener
    void onInterfacesList(const QStringList &lst) override;
    void onConnectionError(const QString &msg) override;

    // ISaveImageListener
    void onImageSaved(const QString &filePath) override;
    void onSaveImageError(const QString &errorMessage) override;

    void errorOccurred(const QString &msg);

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

    int sendProgress() const;
    int receiveProgress() const;

    QString sendProgressText() const;
    QString receiveProgressText() const;

    // worker event slots
    void portOpened(bool ok, const QString &error);
    void packetSent(bool success);
    void packetSendProgress(int sentBytes, int totalBytes);
    void packetReceiveProgress(int receivedBytes, int totalBytes);

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

    std::shared_ptr<SaveImageUseCase> m_saveImageUseCase;
    std::shared_ptr<SendUseCase> m_sendUseCase;
    std::shared_ptr<ConnectionUseCase> m_connectionUseCase;

public:
    void setSaveImageUseCase(std::shared_ptr<SaveImageUseCase> usecase);
    void setSendUseCase(std::shared_ptr<SendUseCase> usecase);
    void setConnectionUseCase(std::shared_ptr<ConnectionUseCase> usecase);
};
