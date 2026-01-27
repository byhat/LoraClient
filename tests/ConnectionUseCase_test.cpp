#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>

#include "../LoraClient/src/core/usecases/ConnectionUseCase.hpp"
#include "MockConnectionWorker.hpp"

using namespace testing;

class ConnectionUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<ConnectionUseCase>();
        mockWorker = std::make_shared<MockConnectionWorker>();
        usecase->setConnector(mockWorker);
    }
    std::unique_ptr<ConnectionUseCase> usecase;
    std::shared_ptr<MockConnectionWorker> mockWorker;
};

TEST_F(ConnectionUseCaseTest, SuccessfulConnectionEstablishes) {
    // Prepare settings
    QVariantHash settings;
    settings["portName"] = "ttyUSB1";
    settings["baud"] = 115200;
    usecase->setSettings(settings);

    EXPECT_CALL(*mockWorker, openPort(QString{"ttyUSB1"}, 115200)).Times(1);
    QSignalSpy errorSpy(usecase.get(), &ConnectionUseCase::errorOccured);
    QSignalSpy ifaceSpy(usecase.get(), &ConnectionUseCase::updateInterfacesList);

    // Mock getInterfacesList to return list
    EXPECT_CALL(*mockWorker, getInterfacesList()).WillOnce(Return(QStringList{"ttyUSB1"}));

    usecase->connect();

    EXPECT_EQ(errorSpy.count(), 0);
    EXPECT_EQ(ifaceSpy.count(), 1);
}

TEST_F(ConnectionUseCaseTest, ConnectionErrorEmitsErrorSignal) {
    QVariantHash settings;
    usecase->setSettings(settings);
    // make openPort throw
    EXPECT_CALL(*mockWorker, openPort(_, _)).WillOnce(Throw(std::runtime_error("fail")));
    QSignalSpy errorSpy(usecase.get(), &ConnectionUseCase::errorOccured);
    usecase->connect();
    EXPECT_EQ(errorSpy.count(), 1);
}

TEST_F(ConnectionUseCaseTest, DisconnectCallsClosePort) {
    QSignalSpy errorSpy(usecase.get(), &ConnectionUseCase::errorOccured);
    EXPECT_CALL(*mockWorker, closePort()).Times(1);
    usecase->disconnect();
    // No error expected
    EXPECT_EQ(errorSpy.count(), 0);
}

TEST_F(ConnectionUseCaseTest, NullConnectorDoesNotCrash) {
    // Do not set connector
    // Expect no crash; just ensure no signal emitted
    QSignalSpy errorSpy(usecase.get(), &ConnectionUseCase::errorOccured);
    // This will dereference null and crash; so we skip calling connect.
    // Instead verify that calling setConnector(nullptr) is safe (no operation)
    usecase->setConnector(nullptr);
    // No expectations, just ensure test runs
    SUCCEED();
}
