#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>

#include <QStringList>

#include "../LoraClient/src/core/usecases/SendUseCase.hpp"
#include "../LoraClient/src/core/usecases/ConnectionUseCase.hpp"
#include "../LoraClient/src/presentation/controller/QmlController.hpp"
#include "MockConnectionWorker.hpp"

using namespace testing;

class QmlControllerTest : public ::testing::Test {
protected:
    void SetUp() override {
        controller = std::make_unique<QmlController>();
        sendUseCase = std::make_shared<SendUseCase>();
        connectionUseCase = std::make_shared<ConnectionUseCase>();
        mockWorker = std::make_shared<MockConnectionWorker>();

        sendUseCase->setConnector(mockWorker);
        connectionUseCase->setConnector(mockWorker);

        controller->setSendUseCase(sendUseCase);
        controller->setConnectionUseCase(connectionUseCase);

        connectionUseCase->setListener(controller.get());
    }

    std::unique_ptr<QmlController> controller;
    std::shared_ptr<SendUseCase> sendUseCase;
    std::shared_ptr<ConnectionUseCase> connectionUseCase;
    std::shared_ptr<MockConnectionWorker> mockWorker;
};

TEST_F(QmlControllerTest, OnSendTextCallsSendUseCaseDirectly) {
    EXPECT_CALL(*mockWorker, sendPacket(_)).Times(1);

    QSignalSpy messagesSpy(controller.get(), &QmlController::messagesChanged);

    controller->onSendText("Hello LoRa!");

    EXPECT_EQ(messagesSpy.count(), 1);
}

TEST_F(QmlControllerTest, OnOpenPortCallsConnectionUseCaseConnect) {
    controller->setProperty("portName", QString("ttyUSB1"));
    controller->setProperty("baudRate", 115200);

    EXPECT_CALL(*mockWorker, openPort(QString{"ttyUSB1"}, 115200)).Times(1);

    controller->onOpenPort();
}

TEST_F(QmlControllerTest, OnClosePortCallsConnectionUseCaseDisconnect) {
    EXPECT_CALL(*mockWorker, closePort()).Times(1);

    controller->onClosePort();
}

TEST_F(QmlControllerTest, OnGetInterfacesListRequestsAndReceivesList) {
    QStringList ports{"ttyUSB0", "ttyUSB1"};
    EXPECT_CALL(*mockWorker, getInterfacesList()).WillOnce(Return(ports));

    QSignalSpy portsSpy(controller.get(), &QmlController::availablePortsChanged);

    controller->onGetInterfacesList();

    EXPECT_EQ(portsSpy.count(), 1);
    EXPECT_EQ(controller->property("availablePorts").toStringList(), ports);
}

TEST_F(QmlControllerTest, OnSendImageWithInvalidPathDoesNotTouchGateway) {
    EXPECT_CALL(*mockWorker, sendPacket(_)).Times(0);

    controller->onSendImage("/nonexistent/path/to/image.png");
}

TEST_F(QmlControllerTest, MissingUseCasesDoNotCrash) {
    QmlController bareController;

    bareController.onSendText("msg");
    bareController.onSendFile("file.txt");
    bareController.onSendImage("/nonexistent/image.png");
    bareController.onOpenPort();
    bareController.onClosePort();
    bareController.onGetInterfacesList();

    SUCCEED();
}
