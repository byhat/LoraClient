#include <gtest/gtest.h>
#include <QFile>
#include <QTemporaryFile>
#include <QImage>

#include "../LoraClient/src/core/usecases/SendUseCase.hpp"
#include "../LoraClient/src/core/entities/MsgStructures.hpp"
#include "MockConnectionWorker.hpp"
#include "MockListeners.hpp"

using namespace testing;

class SendUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<SendUseCase>();
        mockWorker = std::make_shared<MockConnectionWorker>();
        usecase->setConnector(mockWorker);
        usecase->setListener(&listener);
    }
    std::unique_ptr<SendUseCase> usecase;
    std::shared_ptr<MockConnectionWorker> mockWorker;
    MockSendListener listener;
};

TEST_F(SendUseCaseTest, SendTextCallsSendPacket) {
    QString testMessage = "Hello LoRa!";

    // Expect sendPacket to be called with a packet starting with Text type
    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::Text);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendText(testMessage);
}

TEST_F(SendUseCaseTest, SendTextWithEmptyMessage) {
    QString emptyMessage = "";

    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::Text);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendText(emptyMessage);
}

TEST_F(SendUseCaseTest, SendImageWithValidFile) {
    // Create a temporary image file
    QTemporaryFile tempFile;
    tempFile.open();

    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::blue);
    testImage.save(&tempFile, "PNG");
    tempFile.flush();
    tempFile.close();

    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::Image);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendImage(tempFile.fileName());
}

TEST_F(SendUseCaseTest, SendImageWithInvalidFile) {
    QString invalidPath = "/nonexistent/path/to/image.png";

    EXPECT_CALL(*mockWorker, sendPacket(_)).Times(0);

    QString errorMsg;
    EXPECT_CALL(listener, onSendError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->sendImage(invalidPath);

    EXPECT_TRUE(errorMsg.contains("Could not open image file"));
}

TEST_F(SendUseCaseTest, SendFileWithValidFile) {
    // Create a temporary file
    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.write("Test file content");
    tempFile.flush();
    tempFile.close();

    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1 + FileMsg::FILENAME_SIZE);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::File);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendFile(tempFile.fileName());
}

TEST_F(SendUseCaseTest, SendFileWithInvalidPath) {
    QString invalidPath = "/nonexistent/path/to/file.txt";

    EXPECT_CALL(*mockWorker, sendPacket(_)).Times(0);

    QString errorMsg;
    EXPECT_CALL(listener, onSendError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->sendFile(invalidPath);

    EXPECT_TRUE(errorMsg.contains("Could not open file"));
}

TEST_F(SendUseCaseTest, SendFileWithLongFilename) {
    // Create a temporary file with a long name
    QTemporaryFile tempFile("very_long_filename_that_exceeds_the_limit_XXXXXXXXXXXXXXXXXXXXXXXXXXXXX.txt");
    tempFile.open();
    tempFile.write("Test content");
    tempFile.flush();
    tempFile.close();

    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1 + FileMsg::FILENAME_SIZE);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::File);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendFile(tempFile.fileName());
}

TEST_F(SendUseCaseTest, NullConnectorEmitsErrorSignal) {
    usecase->setConnector(nullptr);
    QString testMessage = "Hello LoRa!";
    QString errorMsg;
    EXPECT_CALL(*mockWorker, sendPacket(_)).Times(0);
    EXPECT_CALL(listener, onSendError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->sendText(testMessage);

    EXPECT_TRUE(errorMsg.contains("Gateway adaprer is not initialized"));
}

TEST_F(SendUseCaseTest, MultipleSendOperations) {
    QString testMessage = "Test message";

    // Test sending multiple text messages
    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(3)
        .WillRepeatedly([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::Text);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendText(testMessage);
    usecase->sendText(testMessage + " 2");
    usecase->sendText(testMessage + " 3");
}

TEST_F(SendUseCaseTest, SendEmptyImageFile) {
    // Create an empty temporary file
    QTemporaryFile tempFile;
    tempFile.open();
    tempFile.close();

    EXPECT_CALL(*mockWorker, sendPacket(_))
        .Times(1)
        .WillOnce([](const QByteArray &data) {
            EXPECT_GE(data.size(), 1);
            EXPECT_EQ(static_cast<int>(data[0]), AppEnums::MSG_TYPE::Image);
        });

    EXPECT_CALL(listener, onSendError(_)).Times(0);

    usecase->sendImage(tempFile.fileName());
}
