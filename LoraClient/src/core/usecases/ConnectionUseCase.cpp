#include <mutex>

#include "ConnectionUseCase.hpp"

void ConnectionUseCase::setConnector(std::shared_ptr<IConnectionWorker> connector)
{
    std::unique_lock lock(rw_mutex);
    m_connector = connector;
}

void ConnectionUseCase::setSettings(ConnectionSettings settings)
{
    m_settings = settings;
}

void ConnectionUseCase::setListener(IConnectionListener *listener)
{
    m_listener = listener;
}

void ConnectionUseCase::connect()
{
    std::shared_lock lock(rw_mutex);

    if (!m_connector) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on openPort");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
        return;
    }

    try {
        m_connector->openPort(m_settings.portName, m_settings.baud);
    } catch(...) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on openPort");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
    }
}

void ConnectionUseCase::disconnect()
{
    std::shared_lock lock(rw_mutex);

    if (!m_connector) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on closePort");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
        return;
    }

    try {
        m_connector->closePort();
    } catch(...) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on closePort");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
    }
}

void ConnectionUseCase::setLogger(infrastructure::ILoggerPtr logger) {
    m_logger = logger;
}

void ConnectionUseCase::getInterfacesList()
{
    std::shared_lock lock(rw_mutex);

    if (!m_connector) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on getInterfacesList");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
        if (m_listener) m_listener->onInterfacesList(QStringList{});
        return;
    }

    try {
        QStringList lst = m_connector->getInterfacesList();
        if (m_listener) m_listener->onInterfacesList(lst);
        return;
    } catch(...) {
        if (m_logger) m_logger->log(infrastructure::LogLevel::Error, "Gateway adapter not initialized on getInterfacesList");
        if (m_listener) m_listener->onConnectionError("Gateway adaprer is not initialized");
    }

    if (m_listener) m_listener->onInterfacesList(QStringList{});
}
