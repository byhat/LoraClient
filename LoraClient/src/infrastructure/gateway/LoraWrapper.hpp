#include "src/domain/interfaces/IConnectionWorker.hpp"
#include "src/infrastructure/gateway/LoRaCore/src/LoRaWorker.hpp"


class LoraWrapper : public IConnectionWorker {
    Q_OBJECT
public:
    explicit LoRaWorker(QObject *parent = nullptr) {
        m_loraWorker = std::make_unique<LoRaWorker>(new LoRaWorker(parent));
    };

public slots:
    void openPort(const QString &portName, qint32 baud = 9600) override {
        try {
            m_loraWorker->openPort(portName, baud);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

    void closePort() override {
        try {
            m_loraWorker->closePort();
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }
    void sendPacket(const QByteArray &data) override {
        try {
            m_loraWorker->sendPacket(data);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

private:
    std::unique_ptr<LoRaWorker> m_loraWorker;
};
