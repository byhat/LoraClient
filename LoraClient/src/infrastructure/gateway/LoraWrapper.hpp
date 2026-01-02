#pragma once

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
            m_loraWorker->openPort(portName, baud);
        } catch(...) {
            emit errorOccured("LoRaWorker is not initialized");
        }
    }

    /**
     * @brief Закрывает последовательный порт
     */
    void closePort() noexcept override {
        try {
            m_loraWorker->closePort();
        } catch(...) {
            emit errorOccured("LoRaWorker is not initialized");
        }
    }

    /**
     * @brief Отправляет пакет данных через LoRa-устройство
     *
     * @param data Данные для отправки
     */
    void sendPacket(const QByteArray &data) override {
        try {
            m_loraWorker->sendPacket(data);
        } catch(...) {
            emit errorOccured("LoRaWorker is not initialized");
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
    std::unique_ptr<LoRaWorker> m_loraWorker;  //!< Владеющий указатель на внутренний LoRaWorker
};
