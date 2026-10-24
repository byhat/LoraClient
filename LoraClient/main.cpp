#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#ifdef Q_OS_ANDROID
#include <QCoreApplication>
#include <QJniEnvironment>
#include <cs/SerialPort.hpp>
#endif

#include "src/application/AppEngine.hpp"

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    // Hands the Android Context to CrossSerial's embedded Java bridge before
    // the first cs::SerialPort is touched; without it open() cannot raise the
    // USB permission dialog and port enumeration returns nothing.
    cs::initAndroid(QJniEnvironment::javaVM(),
                    QNativeInterface::QAndroidApplication::context().object());
#endif

    QGuiApplication app(argc, argv);
    AppEngine appEngine;
    appEngine.Init();

    return app.exec();
}
