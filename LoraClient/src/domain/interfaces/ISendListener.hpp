#pragma once

#include <QString>

class ISendListener {
public:
    virtual ~ISendListener() = default;

    virtual void onSendError(const QString &error) = 0;
};
