#pragma once

#include <mutex>

#include <QString>

#include <src/domain/interfaces/IConnectionWorker.hpp>
#include <src/domain/interfaces/ISendListener.hpp>
#include <src/infrastructure/loggining/ILogger.hpp>


class SendUseCase {
public:
    SendUseCase() = default;

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setListener(ISendListener *listener);
    void setLogger(infrastructure::ILoggerPtr logger);
    void sendText(QString msg);
    void sendImage(QString path);
    void sendFile(QString path);

private:
    std::shared_ptr<IConnectionWorker> m_connector;
    ISendListener *m_listener = nullptr;
    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex; //!< блокировка при обновлении коннектора
};
