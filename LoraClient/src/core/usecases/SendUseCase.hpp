#include <QString>
#include <src/domain/interfaces/IConnectionWorker.hpp>


class SendUseCase {
public:
    void setConnector(std::shared_ptr<IConnectionWorker> connector);
    void sendText(QString msg);
    void sendImage(QString path);
    void sendFile(QString path);

private:
    std::shared_ptr<IConnectionWorker> connector;

signals:
    errorOccured(QString error);
};
