#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "src/application/AppEngine.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    AppEngine appEngine;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("appEngine", &appEngine);


#ifdef LET_IT_SNOW
    qDebug() << "Let it snow";
    const QUrl url("qrc:/AppQml/presentation/qml/Main_Snow.qml");
#else
    const QUrl url("qrc:/AppQml/presentation/qml/Main.qml");
#endif

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
