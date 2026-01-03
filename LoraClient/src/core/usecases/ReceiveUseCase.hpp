#pragma once

#include <mutex>

#include <QImage>
#include <QString>
#include <QByteArray>

#include "src/core/entities/MsgStructures.hpp"
#include <src/domain/interfaces/IConnectionWorker.hpp>


class ReceiveUseCase : public QObject{
    Q_OBJECT
public:
    explicit ReceiveUseCase(QObject *parent = nullptr);

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void handleData(const QByteArray &data);

signals:
    void imageReceived(const ImageMsg &img);
    void txtReceived(const TextMsg &str);
    void fileReceived(const FileMsg &data);
    void errorOccured(QString error);

private:
    std::shared_ptr<IConnectionWorker> m_connector;
    std::mutex m_mutex; //!< блокировка при обновлении коннектора
};
