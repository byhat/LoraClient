#include <mutex>

#include <QFile>
#include <QFileInfo>

#include "SendUseCase.hpp"
#include "src/core/entities/MsgStructures.hpp"


void SendUseCase::setConnector(std::shared_ptr<IConnectionWorker> connector)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connector = connector;
}

void SendUseCase::setListener(ISendListener *listener)
{
    m_listener = listener;
}

void SendUseCase::setLogger(infrastructure::ILoggerPtr logger) {
    m_logger = logger;
}

void SendUseCase::sendText(QString msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    QByteArray packet;
    packet.push_back(AppEnums::MSG_TYPE::Text);
    packet.push_back(qCompress(msg.toUtf8(), 9));
    if (!m_connector) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
        return;
    }
    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
    }
}

void SendUseCase::sendImage(QString path)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    QByteArray packet;
    packet.push_back(AppEnums::MSG_TYPE::Image);

    QFile file(path);

    bool openStatus = file.open(QFile::ReadOnly);
    if (!openStatus) {
        if (m_listener) m_listener->onSendError("Could not open image file");
        return;
    }

    packet.push_back(qCompress(file.readAll(), 9));

    if (!m_connector) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
        return;
    }
    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
    }
}

void SendUseCase::sendFile(QString path)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    QByteArray packet;
    packet.push_back(AppEnums::MSG_TYPE::File);

    QFile file(path);

    bool openStatus = file.open(QFile::ReadOnly);
    if (!openStatus) {
        if (m_listener) m_listener->onSendError("Could not open file");
        return;
    }

    QByteArray nameField = QFileInfo(path).fileName()
                               .toUtf8()
                               .left(FileMsg::FILENAME_SIZE);

    nameField.resize(FileMsg::FILENAME_SIZE);

    packet.append(nameField);
    packet.push_back(qCompress(file.readAll(), 9));

    if (!m_connector) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
        return;
    }
    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        if (m_listener) m_listener->onSendError("Gateway adaprer is not initialized");
    }
}
