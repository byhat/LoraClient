#include <mutex>
#include "ConnectionUseCase.hpp"

ConnectionUseCase::ConnectionUseCase(QObject *parent)
    : QObject {parent}
{

}

void ConnectionUseCase::setConnector(std::shared_ptr<IConnectionWorker> connector)
{
    std::unique_lock lock(rw_mutex);
    m_connector = connector;
}

void ConnectionUseCase::setSettings(QVariantHash settings)
{
    m_settings = settings;
}

void ConnectionUseCase::connect()
{
    std::shared_lock lock(rw_mutex);

    QString portName = "ttyUSB0";
    qint32 baud = 9600;

    if (m_settings.contains("portName")) {
        portName = m_settings["portName"].toString();
    }

    if (m_settings.contains("baud")) {
        baud = m_settings["baud"].toUInt();
    }

    try {
        m_connector->openPort(portName, baud);
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
    }
}

void ConnectionUseCase::disconnect()
{
    std::shared_lock lock(rw_mutex);

    try {
        m_connector->closePort();
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
    }
}

void ConnectionUseCase::getInterfacesList()
{
    std::shared_lock lock(rw_mutex);

    try {
        emit updateInterfacesList(m_connector->getInterfacesList());
        return;
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
    }

    emit updateInterfacesList(QStringList{});
}
