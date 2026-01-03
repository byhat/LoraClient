#pragma once

#include <shared_mutex>

#include <QStringList>
#include <QVariantHash>

#include <src/domain/interfaces/IConnectionWorker.hpp>


class ConnectionUseCase : public QObject {
    Q_OBJECT
public:
    explicit ConnectionUseCase(QObject *parent = nullptr);

    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setSettings(QVariantHash settings);
    void connect();
    void disconnect();
    void getInterfacesList();

signals:
    void errorOccured(QString error);
    void updateInterfacesList(QStringList lst);

private:
    QVariantHash m_settings;
    std::shared_ptr<IConnectionWorker> m_connector;

    std::shared_mutex rw_mutex; //!< блокировка при обновлении коннектора
};
