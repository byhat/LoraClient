#include "ReceiveUseCase.hpp"
#include <QDebug>

ReceiveUseCase::ReceiveUseCase(QObject *parent)
    : QObject {parent}
{

}

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

void ReceiveUseCase::setLogger(infrastructure::ILoggerPtr logger) {
    m_logger = logger;
}

void ReceiveUseCase::handleData(const QByteArray &data)
{
    qDebug() << "handleData: received data size =" << data.size() << ", type =" << (data.isEmpty() ? -1 : static_cast<int>(data[0]));

    if (data.isEmpty()) {
        qDebug() << "Warning: Received empty data packet";
        return;
    }

    int type = static_cast<int>(data[0]);

    if (type == AppEnums::MSG_TYPE::Text) {
        TextMsg msg;
        msg.text = qUncompress(data.mid(AppEnums::MSG_TYPE_FLAG_SIZE));
        msg.time = QDateTime::currentDateTime();
        emit txtReceived(msg);
    } else if (type == AppEnums::MSG_TYPE::Image) {
        ImageMsg msg;
        QByteArray uncompressedData = qUncompress(data.mid(AppEnums::MSG_TYPE_FLAG_SIZE));
        qDebug() << "handleData: Image data, uncompressed size =" << uncompressedData.size();
        bool loaded = msg.img.loadFromData(uncompressedData);
        qDebug() << "handleData: Image loadFromData result =" << loaded << ", isNull =" << msg.img.isNull();
        if (!loaded || msg.img.isNull()) {
            qDebug() << "Warning: Failed to load image from data";
        }
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
