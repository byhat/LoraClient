#pragma once

#include <QObject>
#include <QVariant>
#include <QByteArray>
#include <QString>

#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/core/usecases/SendUseCase.hpp"
#include "src/core/usecases/ReceiveUseCase.hpp"
#include "src/core/usecases/ConnectionUseCase.hpp"

#include "src/domain/interfaces/IConnectionWorker.hpp"
#include "src/presentation/controller/QmlController.hpp"


class AppEngine : public QObject
{
    Q_OBJECT

public:
    explicit AppEngine(QObject *parent = nullptr);
    void Init();

private:
    void setupQmlEngine();

    void setConnector();
    void setupConnections();

    void setupReceiveUcConnections();
    void setupSendUcConnections();
    void setupConnectionUcConnections();
    void setupConnectionWConnections();

    std::shared_ptr<IConnectionWorker> m_connector;
    std::unique_ptr<QmlController> m_controller;

    std::unique_ptr<SendUseCase> m_sendUseCase;
    std::unique_ptr<ReceiveUseCase> m_receiveUseCase;
    std::unique_ptr<ConnectionUseCase> m_connectionUseCase;

    std::unique_ptr<QQmlApplicationEngine> m_engine;

};
