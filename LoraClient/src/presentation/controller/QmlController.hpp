#pragma once

class QmlPresenter : QObject {
    Q_OBJECT

    Q_PROPERTY(QString portName READ portName WRITE setPortName NOTIFY portNameChanged)
    Q_PROPERTY(qint32 baudRate READ baudRate WRITE setBaudRate NOTIFY baudRateChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)
    Q_PROPERTY(QString lastError READ lastError NOTIFY errorOccurred)
    Q_PROPERTY(QVariantList availablePorts READ availablePorts NOTIFY availablePortsChanged)

    Q_PROPERTY(QVariantList messages READ messages NOTIFY messagesChanged)

    Q_PROPERTY(int sendProgress READ sendProgress NOTIFY sendProgressChanged)
    Q_PROPERTY(int receiveProgress READ receiveProgress NOTIFY receiveProgressChanged)

    Q_PROPERTY(QString sendProgressText READ sendProgressText NOTIFY sendProgressChanged)
    Q_PROPERTY(QString receiveProgressText READ receiveProgressText NOTIFY receiveProgressChanged)

public slots:

    void sendTxt();
    void sendImage(QString path);
    void sendFile(QString path);
    void showImage(const &QImage img);
    void showTxt(const QString &txt);
    void showFileMetadata(const QString &txt);

    void openPort();
    void closePort();
    Q_INVOKABLE void updateAvailablePorts();

private:
    QString m_portName;
    qint32 m_baudRate;
    bool m_isConnected;
    QString m_lastError;
    QVariantList m_availablePorts;

    QVariantList m_messages;
    LoRaWorker *m_loRaWorker = nullptr;

    int m_sendProgress = 0;
    int m_receiveProgress = 0;

    int m_sendBytes = 0;
    int m_sendTotal = 0;
    int m_receiveBytes = 0;
    int m_receiveTotal = 0;

    void addMessage(const QString &text, const QString &type);
    void initializeLoRaWorker();
};
