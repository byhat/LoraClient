#include <QStringList>
#include <QVariantHash>

#include <src/domain/interfaces/IConnectionWorker.hpp>


class ConnectionUseCase {
public:
    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void setSettings(QVariantHash settings);
    void connect();
    void disconnect();
    QStringList getInterfacesList();

private:
    std::shared_ptr<IConnectionWorker> connector;

signals:
    errorOccured(QString error);
};
