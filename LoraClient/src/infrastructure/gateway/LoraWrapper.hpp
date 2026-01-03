#pragma once

#include <QThread>
#include <QByteArray>
#include <QSerialPortInfo>

#include "src/domain/interfaces/IConnectionWorker.hpp"
#include "src/infrastructure/gateway/LoRaCore/src/LoRaWorker.hpp"


/**
 * @brief Обёртка над LoRaWorker, реализующая интерфейс IConnectionWorker
 *
 * Предоставляет расширенный интерфейс для работы с LoRa-устройством через последовательный порт
 */
class LoraWrapper : public IConnectionWorker {
    Q_OBJECT
public:
    /**
     * @brief Конструктор класса
     *
     * @param parent Указатель на родительский QObject
     */
    explicit LoraWrapper(QObject *parent = nullptr) {
        m_loraWorker = std::make_unique<LoRaWorker>(parent);
        // m_loraWorker = std::make_unique<LoRaWorker>();
        // m_loraWorker->moveToThread(&m_workerThread);

        connectSignals();
    }

    ~LoraWrapper() = default;

public slots:
    /**
     * @brief Открывает последовательный порт для связи с LoRa-устройством
     *
     * @param portName Имя порта (например, "COM3", "/dev/ttyUSB0")
     * @param baud Скорость передачи данных в бодах (по умолчанию 9600)
     */
    void openPort(const QString &portName, qint32 baud = 9600) override {
        try {
            // QMetaObject::invokeMethod(
            //     m_loraWorker.get(),
            //     "openPort",
            //     Qt::QueuedConnection
            //     );
            m_loraWorker->openPort(portName, baud);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

    /**
     * @brief Закрывает последовательный порт
     */
    void closePort() noexcept override {
        try {
            // QMetaObject::invokeMethod(
            //     m_loraWorker.get(),
            //     "closePort",
            //     Qt::QueuedConnection
            //     );
            m_loraWorker->closePort();
            emit portOpened(false);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

    /**
     * @brief Отправляет пакет данных через LoRa-устройство
     *
     * @param data Данные для отправки
     */
    void sendPacket(const QByteArray &data) override {
        try {
            // QMetaObject::invokeMethod(
            //     m_loraWorker.get(),
            //     "sendPacket",
            //     Qt::QueuedConnection,
            //     Q_ARG(QByteArray, data)
            //     );
            m_loraWorker->sendPacket(data);
        } catch(...) {
            emit errorOccurred("LoRaWorker is not initialized");
        }
    }

    /**
     * @brief Возвращает список доступных последовательных портов
     *
     * @return QStringList Список имён доступных портов
     */
    QStringList getInterfacesList() override {
        QList<QSerialPortInfo> lst = QSerialPortInfo::availablePorts();

        QStringList ports;
        ports.reserve(lst.size());

        for (auto& port : lst) {
            ports.push_back(port.portName());
        }

        return ports;
    }

private:
    void connectSignals() {
        connect(m_loraWorker.get(),
                &LoRaWorker::portOpened,
                this,
                [this](bool ok, const QString &error) {
                    emit portOpened(ok, error);
                },
                Qt::QueuedConnection);

        connect(m_loraWorker.get(),
                &LoRaWorker::packetSent,
                this,
                [this](bool success) {
                    emit packetSent(success);
                },
                Qt::QueuedConnection);

        connect(m_loraWorker.get(),
                &LoRaWorker::packetReceived,
                this,
                [this](const QByteArray &data) {
                    emit packetReceived(data);
                },
                Qt::QueuedConnection);

        connect(m_loraWorker.get(),
                &LoRaWorker::errorOccurred,
                this,
                [this](const QString &msg) {
                    emit errorOccurred(msg);
                },
                Qt::QueuedConnection);

        connect(m_loraWorker.get(),
                &LoRaWorker::packetSendProgress,
                this,
                [this](int sentBytes, int totalBytes) {
                    emit packetSendProgress(sentBytes,
                                            totalBytes);
                },
                Qt::QueuedConnection);

        connect(m_loraWorker.get(),
                &LoRaWorker::packetReceiveProgress,
                this,
                [this](int receivedBytes, int totalBytes) {
                    emit packetReceiveProgress(receivedBytes,
                                            totalBytes);
                },
                Qt::QueuedConnection);
    }

    QThread m_workerThread;
    std::unique_ptr<LoRaWorker> m_loraWorker;  //!< Владеющий указатель на внутренний LoRaWorker
};
