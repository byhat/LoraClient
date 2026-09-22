#pragma once

#include <QString>

#include "src/core/entities/MsgStructures.hpp"

class IReceiveListener {
public:
    virtual ~IReceiveListener() = default;

    virtual void onTextReceived(const TextMsg &msg) = 0;
    virtual void onImageReceived(const ImageMsg &msg) = 0;
    virtual void onFileReceived(const FileMsg &msg) { (void)msg; }
    virtual void onReceiveError(const QString &error) = 0;
};
