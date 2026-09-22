#pragma once

#include <mutex>

#include <QImage>
#include <QString>
#include <QByteArray>

#include "src/core/entities/MsgStructures.hpp"
#include <src/domain/interfaces/IConnectionWorker.hpp>
#include <src/domain/interfaces/IReceiveListener.hpp>
#include <src/infrastructure/loggining/ILogger.hpp>


class ReceiveUseCase{
public:
    ReceiveUseCase() = default;

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setListener(IReceiveListener *listener);
    void setLogger(infrastructure::ILoggerPtr logger);
    void handleData(const QByteArray &data);

private:
    std::shared_ptr<IConnectionWorker> m_connector;
    IReceiveListener *m_listener = nullptr;
    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex; //!< блокировка при обновлении коннектора
};
