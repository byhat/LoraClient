#include "ReceiveUseCase.hpp"

void ReceiveUseCase::setConnector(std::shared_ptr<IConnectionWorker> connector)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_connector = connector;

    try {
        connect(m_connector.get(),
                &IConnectionWorker::packetReceived,
                this,
                &ReceiveUseCase::handleData,
                Qt::QueuedConnection);
    } catch(...) {
        emit errorOccured("Gateway adaprer is not initialized");
    }
}

void ReceiveUseCase::handleData(QByteArray data)
{
    int type = static_cast<int>(data[0]);

    if (type == AppEnums::MSG_TYPE::Text) {
        TextMsg msg;
        msg.text = qUncompress(data.mid(AppEnums::MSG_TYPE_FLAG_SIZE));
        msg.time = QDateTime::currentDateTime();
        emit txtReceived(msg);
    } else if (type == AppEnums::MSG_TYPE::Image) {
        ImageMsg msg;
        msg.img = QImage(qUncompress(data.mid(AppEnums::MSG_TYPE_FLAG_SIZE)));
        msg.time = QDateTime::currentDateTime();
        emit imageReceived(msg);
    } else if (type == AppEnums::MSG_TYPE::File) {
        FileMsg msg;
        msg.fileName = QString(qUncompress(data.mid(1, FileMsg::FILENAME_SIZE)));
        msg.data = qUncompress(data.mid(AppEnums::MSG_TYPE_FLAG_SIZE));
        msg.time = QDateTime::currentDateTime();
        emit fileReceived(msg);
    }
}
