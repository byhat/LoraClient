#include "AppEngine.hpp"
#include <QDateTime>
#include <QVariantList>
#include <QVariantMap>
#include "QCrossPlatformSerialPortInfo.hpp"
#include <QCoreApplication>

#include "src/infrastructure/gateway/LoraWrapper.hpp"
#include "src/infrastructure/loggining/SpdlogLogger.hpp"

AppEngine::AppEngine(QObject *parent)
    : QObject { parent }
    , m_connector         { std::make_shared<LoraWrapper>(this)                }
    , m_logger            { std::make_shared<infrastructure::SpdlogLogger>()   }
    , m_sendUseCase       { std::make_shared<SendUseCase>()                  }
    , m_receiveUseCase    { std::make_unique<ReceiveUseCase>()               }
    , m_connectionUseCase { std::make_shared<ConnectionUseCase>()             }
    , m_saveImageUseCase  { std::make_shared<SaveImageUseCase>()              }
    , m_controller        { std::make_unique<QmlController>(this)              }
    , m_engine            { std::make_unique<QQmlApplicationEngine>(this)      }
{

}

void AppEngine::Init()
{
    setLogger();
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

void AppEngine::setLogger()
{
    if (m_logger) {
        m_sendUseCase->setLogger(m_logger);
        m_receiveUseCase->setLogger(m_logger);
        m_connectionUseCase->setLogger(m_logger);
        m_saveImageUseCase->setLogger(m_logger);
    }
}

void AppEngine::setConnector()
{
    m_sendUseCase->setConnector(m_connector);
    m_receiveUseCase->setConnector(m_connector);
    m_connectionUseCase->setConnector(m_connector);
    if (m_logger) {
        m_logger->log(infrastructure::LogLevel::Info, "Connector set in AppEngine");
    }
}

void AppEngine::setupConnections()
{
    setupReceiveUcConnections();
    setupConnectionUcConnections();
    setupConnectionWConnections();
    setupSaveImageUcConnections();
    setupControllerUseCases();
}

void AppEngine::setupReceiveUcConnections()
{
    connect(m_connector.get(),
            &IConnectionWorker::packetReceived,
            this,
            [this](const QByteArray &data) { m_receiveUseCase->handleData(data); },
            Qt::QueuedConnection);

    m_receiveUseCase->setListener(m_controller.get());
}

void AppEngine::setupConnectionUcConnections()
{
    m_connectionUseCase->setListener(m_controller.get());
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

void AppEngine::setupSaveImageUcConnections()
{
    m_saveImageUseCase->setListener(m_controller.get());
    m_controller->setSaveImageUseCase(m_saveImageUseCase);
}

void AppEngine::setupControllerUseCases()
{
    m_controller->setSendUseCase(m_sendUseCase);
    m_controller->setConnectionUseCase(m_connectionUseCase);
    m_sendUseCase->setListener(m_controller.get());
}
