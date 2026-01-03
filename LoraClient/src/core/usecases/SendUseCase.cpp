#include <mutex>

#include <QFile>
#include <QFileInfo>

#include "SendUseCase.hpp"
#include "src/core/entities/MsgStructures.hpp"


SendUseCase::SendUseCase(QObject *parent)
    : QObject {parent}
{

}

void SendUseCase::setConnector(std::shared_ptr<IConnectionWorker> connector)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connector = connector;
}

void SendUseCase::sendText(QString msg)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    QByteArray packet;
    packet.push_back(AppEnums::MSG_TYPE::Text);
    packet.push_back(qCompress(msg.toUtf8(), 9));
    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
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
        emit errorOccured("Could not open image file");
        return;
    }

    packet.push_back(qCompress(file.readAll(), 9));

    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
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
        emit errorOccured("Could not open file");
        return;
    }

    QByteArray nameField = QFileInfo(path).fileName()
                               .toUtf8()
                               .left(FileMsg::FILENAME_SIZE);

    nameField.resize(FileMsg::FILENAME_SIZE);

    packet.append(nameField);
    packet.push_back(qCompress(file.readAll(), 9));

    try {
        m_connector->sendPacket(packet);
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
    }
}
