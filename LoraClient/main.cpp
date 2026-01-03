#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "src/application/AppEngine.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    AppEngine appEngine;
    appEngine.Init();

    return app.exec();
}
