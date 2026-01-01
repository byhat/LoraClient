#include <QImage>
#include <QString>
#include <QByteArray>

#include <src/domain/interfaces/IConnectionWorker.hpp>


class ReceiveUseCase {
public:
    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void handleData(QByteArray data);

signals:
    void imageReceived(const QImage &img);
    void txtReceived(const QString &str);
    void fileReceived(const QByteAray &data);
    errorOccured(QString error);

private:
    std::shared_ptr<IConnectionWorker> connector;

};
