#pragma once

#include <mutex>

#include <QImage>
#include <QString>
#include <QByteArray>

#include "src/core/entities/MsgStructures.hpp"
#include <src/domain/interfaces/IConnectionWorker.hpp>
#include <src/infrastructure/loggining/ILogger.hpp>


class ReceiveUseCase : public QObject{
    Q_OBJECT
public:
    explicit ReceiveUseCase(QObject *parent = nullptr);

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setLogger(infrastructure::ILoggerPtr logger);
    void handleData(const QByteArray &data);

signals:
    void imageReceived(const ImageMsg &img);
    void txtReceived(const TextMsg &str);
    void fileReceived(const FileMsg &data);
    void errorOccured(QString error);

private:
    std::shared_ptr<IConnectionWorker> m_connector;
    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex; //!< блокировка при обновлении коннектора
};
