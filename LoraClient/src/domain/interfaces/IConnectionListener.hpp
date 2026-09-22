#pragma once

#include <QString>
#include <QStringList>

class IConnectionListener {
public:
    virtual ~IConnectionListener() = default;

    virtual void onInterfacesList(const QStringList &lst) = 0;
    virtual void onConnectionError(const QString &error) = 0;
};
