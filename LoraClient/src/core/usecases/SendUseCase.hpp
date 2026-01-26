#pragma once

#include <mutex>

#include <QString>

#include <src/domain/interfaces/IConnectionWorker.hpp>
#include <src/infrastructure/loggining/ILogger.hpp>


class SendUseCase : public QObject {
    Q_OBJECT
public:
    explicit SendUseCase(QObject *parent = nullptr);

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setLogger(infrastructure::ILoggerPtr logger);
    void sendText(QString msg);
    void sendImage(QString path);
    void sendFile(QString path);

signals:
    void errorOccured(QString error);

private:
    std::shared_ptr<IConnectionWorker> m_connector;
    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex; //!< блокировка при обновлении коннектора
};
