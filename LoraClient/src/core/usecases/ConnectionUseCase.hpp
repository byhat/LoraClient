#pragma once

#include <shared_mutex>

#include <QStringList>

#include "../../core/entities/ConnectionSettings.hpp"
#include "../../domain/interfaces/IConnectionListener.hpp"
#include "../../domain/interfaces/IConnectionWorker.hpp"
#include "../../infrastructure/loggining/ILogger.hpp"


class ConnectionUseCase {
public:
    ConnectionUseCase() = default;

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setSettings(ConnectionSettings settings);
    void setListener(IConnectionListener *listener);
    void connect();
    void disconnect();
    void getInterfacesList();
    void setLogger(infrastructure::ILoggerPtr logger);

private:
    ConnectionSettings m_settings;
    std::shared_ptr<IConnectionWorker> m_connector;
    IConnectionListener *m_listener = nullptr;
    infrastructure::ILoggerPtr m_logger;
    std::shared_mutex rw_mutex; //!< блокировка при обновлении коннектора
};
