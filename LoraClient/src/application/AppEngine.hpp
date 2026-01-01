#pragma once

#include <QObject>
#include <QVariant>
#include <QByteArray>
#include <QString>

class LoRaWorker;

class AppEngine : public QObject
{
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

public:
    explicit AppEngine(QObject *parent = nullptr);
    ~AppEngine() override;

    QString portName() const;
    void setPortName(const QString &portName);

    qint32 baudRate() const;
    void setBaudRate(qint32 baudRate);

    bool isConnected() const;
    QString lastError() const;

    QVariantList messages() const;
    QVariantList availablePorts() const;

    int sendProgress() const;
    int receiveProgress() const;

    QString sendProgressText() const;
    QString receiveProgressText() const;

public slots:
    void openPort();
    void closePort();
    void sendTextMessage(const QString &text);
    Q_INVOKABLE void updateAvailablePorts();

signals:
    void portNameChanged();
    void baudRateChanged();
    void isConnectedChanged();
    void errorOccurred();
    void messagesChanged();
    void availablePortsChanged();
    void sendProgressChanged();
    void receiveProgressChanged();

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
