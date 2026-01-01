#include "src/domain/interfaces/IConnectionWorker.hpp"
#include "src/infrastructure/gateway/LoRaCore/LoraWorker.hpp"


class LoraWrapper : public IConnectionWorker {
    Q_OBJECT
public:
    explicit LoRaWorker(QObject *parent = nullptr) {
        m_loraWorker = std::make_unique<LoraWorker>(new LoraWorker(parent));
    };

    ~LoRaWorker() = default;

public slots:
    void openPort(const QString &portName, qint32 baud = 9600) {
        try {
            m_loraWorker->openPort(portName, baud);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

    void closePort() {
        try {
            m_loraWorker->closePort();
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }
    void sendPacket(const QByteArray &data) {
        try {
            m_loraWorker->sendPacket(data);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

private:
    std::unique_ptr<LoraWorker> m_loraWorker;
};
