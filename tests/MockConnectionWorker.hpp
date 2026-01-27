#pragma once

#include <gmock/gmock.h>
#include "../LoraClient/src/domain/interfaces/IConnectionWorker.hpp"

class MockConnectionWorker : public IConnectionWorker {
    Q_OBJECT
public:
    MOCK_METHOD(void, openPort, (const QString &portName, qint32 baud), (override));
    MOCK_METHOD(void, closePort, (), (override, noexcept));
    MOCK_METHOD(void, sendPacket, (const QByteArray &data), (override));
    MOCK_METHOD(QStringList, getInterfacesList, (), (override));
};
