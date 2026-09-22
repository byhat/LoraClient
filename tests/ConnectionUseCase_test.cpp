#include <gtest/gtest.h>

#include "../LoraClient/src/core/usecases/ConnectionUseCase.hpp"
#include "MockConnectionWorker.hpp"
#include "MockListeners.hpp"

using namespace testing;

class ConnectionUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<ConnectionUseCase>();
        mockWorker = std::make_shared<MockConnectionWorker>();
        usecase->setConnector(mockWorker);
        usecase->setListener(&listener);
    }
    std::unique_ptr<ConnectionUseCase> usecase;
    std::shared_ptr<MockConnectionWorker> mockWorker;
    MockConnectionListener listener;
};

TEST_F(ConnectionUseCaseTest, SuccessfulConnectionEstablishes) {
    ConnectionSettings settings;
    settings.portName = "ttyUSB1";
    settings.baud = 115200;
    usecase->setSettings(settings);

    EXPECT_CALL(*mockWorker, openPort(QString{"ttyUSB1"}, 115200)).Times(1);
    EXPECT_CALL(listener, onConnectionError(_)).Times(0);

    usecase->connect();
}

TEST_F(ConnectionUseCaseTest, ConnectionErrorNotifiesListener) {
    // make openPort throw
    EXPECT_CALL(*mockWorker, openPort(_, _)).WillOnce(Throw(std::runtime_error("fail")));
    EXPECT_CALL(listener, onConnectionError(_)).Times(1);

    usecase->connect();
}

TEST_F(ConnectionUseCaseTest, DisconnectCallsClosePort) {
    EXPECT_CALL(*mockWorker, closePort()).Times(1);
    EXPECT_CALL(listener, onConnectionError(_)).Times(0);

    usecase->disconnect();
}

TEST_F(ConnectionUseCaseTest, GetInterfacesListNotifiesListener) {
    QStringList ports{"ttyUSB0", "ttyACM0"};
    EXPECT_CALL(*mockWorker, getInterfacesList()).WillOnce(Return(ports));
    EXPECT_CALL(listener, onInterfacesList(ports)).Times(1);

    usecase->getInterfacesList();
}

TEST_F(ConnectionUseCaseTest, GetInterfacesListErrorReturnsEmptyList) {
    EXPECT_CALL(*mockWorker, getInterfacesList()).WillOnce(Throw(std::runtime_error("fail")));
    EXPECT_CALL(listener, onConnectionError(_)).Times(1);
    EXPECT_CALL(listener, onInterfacesList(QStringList{})).Times(1);

    usecase->getInterfacesList();
}

TEST_F(ConnectionUseCaseTest, NullConnectorDoesNotCrash) {
    usecase->setConnector(nullptr);
    SUCCEED();
}
