#include "AppEngine.hpp"
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include <QSerialPortInfo>
#include <QCoreApplication>

#include "src/infrastructure/gateway/LoraWrapper.hpp"


AppEngine::AppEngine(QObject *parent)
    : QObject { parent }
    , m_connector         { std::make_shared<LoraWrapper>(this)           }
    , m_sendUseCase       { std::make_unique<SendUseCase>(this)           }
    , m_receiveUseCase    { std::make_unique<ReceiveUseCase>(this)        }
    , m_connectionUseCase { std::make_unique<ConnectionUseCase>(this)     }
    , m_controller        { std::make_unique<QmlController>(this)         }
    , m_engine            { std::make_unique<QQmlApplicationEngine>(this) }
{

}

void AppEngine::Init()
{
    setConnector();
    setupConnections();

    m_controller->onGetInterfacesList();
    setupQmlEngine();
}

void AppEngine::setupQmlEngine()
{
#ifdef LET_IT_SNOW
    qDebug() << "Let it snow";
    const QUrl url("qrc:/AppQml/qml/Main_Snow.qml");
#else
    const QUrl url("qrc:/AppQml/qml/Main.qml");
#endif

    QObject::connect(m_engine.get(), &QQmlApplicationEngine::objectCreated,
                     this, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    m_engine->rootContext()->setContextProperty("appEngine", m_controller.get());
    m_engine->load(url);
}

void AppEngine::setConnector()
{
    m_sendUseCase->setConnector(m_connector);
    m_receiveUseCase->setConnector(m_connector);
    m_connectionUseCase->setConnector(m_connector);
}

void AppEngine::setupConnections()
{
    setupReceiveUcConnections();
    setupSendUcConnections();
    setupConnectionUcConnections();
    setupConnectionWConnections();
}

void AppEngine::setupReceiveUcConnections()
{
    connect(m_receiveUseCase.get(),
            &ReceiveUseCase::txtReceived,
            m_controller.get(),
            &QmlController::textReceived);

    connect(m_receiveUseCase.get(),
            &ReceiveUseCase::imageReceived,
            m_controller.get(),
            &QmlController::imageReceived);
}

void AppEngine::setupSendUcConnections()
{
    connect(m_controller.get(),
            &QmlController::sendText,
            m_sendUseCase.get(),
            &SendUseCase::sendText);

    connect(m_controller.get(),
            &QmlController::sendFile,
            m_sendUseCase.get(),
            &SendUseCase::sendFile);

    connect(m_controller.get(),
            &QmlController::sendImage,
            m_sendUseCase.get(),
            &SendUseCase::sendImage);
}

void AppEngine::setupConnectionUcConnections()
{
    connect(m_controller.get(),
            &QmlController::setSettings,
            m_connectionUseCase.get(),
            &ConnectionUseCase::setSettings);

    connect(m_controller.get(),
            &QmlController::openPort,
            m_connectionUseCase.get(),
            &ConnectionUseCase::connect);

    connect(m_controller.get(),
            &QmlController::closePort,
            m_connectionUseCase.get(),
            &ConnectionUseCase::disconnect);

    connect(m_controller.get(),
            &QmlController::getInterfacesList,
            m_connectionUseCase.get(),
            &ConnectionUseCase::getInterfacesList);

    connect(m_connectionUseCase.get(),
            &ConnectionUseCase::updateInterfacesList,
            m_controller.get(),
            &QmlController::onUpdateInterfacesList);
}

void AppEngine::setupConnectionWConnections()
{
    connect(m_connector.get(),
            &IConnectionWorker::portOpened,
            m_controller.get(),
            &QmlController::portOpened);

    connect(m_connector.get(),
            &IConnectionWorker::packetSent,
            m_controller.get(),
            &QmlController::packetSent);

    connect(m_connector.get(),
            &IConnectionWorker::packetSendProgress,
            m_controller.get(),
            &QmlController::packetSendProgress);

    connect(m_connector.get(),
            &IConnectionWorker::packetReceiveProgress,
            m_controller.get(),
            &QmlController::packetReceiveProgress);
}
