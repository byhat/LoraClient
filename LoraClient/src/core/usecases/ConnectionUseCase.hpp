#pragma once

#include <shared_mutex>

#include <QStringList>
#include <QVariantHash>

#include <src/domain/interfaces/IConnectionWorker.hpp>


class ConnectionUseCase : public QObject {
    Q_OBJECT
public:
    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setSettings(QVariantHash settings);
    void connect();
    void disconnect();
    QStringList getInterfacesList();

signals:
    void errorOccured(QString error);

private:
    QVariantHash m_settings;
    std::shared_ptr<IConnectionWorker> m_connector;

    std::shared_mutex rw_mutex; //!< блокировка при обновлении коннектора
};
