#pragma once

#include <QString>

struct ConnectionSettings {
    QString portName = "ttyUSB0";
    qint32 baud = 9600;
};
