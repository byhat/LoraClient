#pragma once

#include <gmock/gmock.h>

#include "../LoraClient/src/domain/interfaces/IReceiveListener.hpp"
#include "../LoraClient/src/domain/interfaces/ISendListener.hpp"
#include "../LoraClient/src/domain/interfaces/IConnectionListener.hpp"
#include "../LoraClient/src/domain/interfaces/ISaveImageListener.hpp"

class MockReceiveListener : public IReceiveListener {
public:
    MOCK_METHOD(void, onTextReceived, (const TextMsg &msg), (override));
    MOCK_METHOD(void, onImageReceived, (const ImageMsg &msg), (override));
    MOCK_METHOD(void, onFileReceived, (const FileMsg &msg), (override));
    MOCK_METHOD(void, onReceiveError, (const QString &error), (override));
};

class MockSendListener : public ISendListener {
public:
    MOCK_METHOD(void, onSendError, (const QString &error), (override));
};

class MockConnectionListener : public IConnectionListener {
public:
    MOCK_METHOD(void, onInterfacesList, (const QStringList &lst), (override));
    MOCK_METHOD(void, onConnectionError, (const QString &error), (override));
};

class MockSaveImageListener : public ISaveImageListener {
public:
    MOCK_METHOD(void, onImageSaved, (const QString &filePath), (override));
    MOCK_METHOD(void, onSaveImageError, (const QString &error), (override));
};
