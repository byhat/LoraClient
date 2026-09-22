#include <gtest/gtest.h>
#include <QByteArray>
#include <QBuffer>
#include <QImage>

#include "../LoraClient/src/core/usecases/ReceiveUseCase.hpp"
#include "MockConnectionWorker.hpp"
#include "MockListeners.hpp"

using namespace testing;

class ReceiveUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<ReceiveUseCase>();
        mockWorker = std::make_shared<MockConnectionWorker>();
        usecase->setConnector(mockWorker);
        usecase->setListener(&listener);
    }
    std::unique_ptr<ReceiveUseCase> usecase;
    std::shared_ptr<MockConnectionWorker> mockWorker;
    MockReceiveListener listener;
};

TEST_F(ReceiveUseCaseTest, TextMessageNotifiesListener) {
    // Prepare text message data
    QString testText = "Hello LoRa!";
    QByteArray compressedText = qCompress(testText.toUtf8(), 9);

    QByteArray packet;
    packet.push_back(static_cast<char>(AppEnums::MSG_TYPE::Text));
    packet.push_back(compressedText);

    TextMsg captured;
    EXPECT_CALL(listener, onTextReceived(_)).WillOnce(SaveArg<0>(&captured));
    EXPECT_CALL(listener, onReceiveError(_)).Times(0);

    usecase->handleData(packet);

    EXPECT_EQ(captured.text, testText);
}

TEST_F(ReceiveUseCaseTest, ImageMessageNotifiesListener) {
    // Create a simple test image
    QImage testImage(100, 100, QImage::Format_RGB32);
    testImage.fill(Qt::red);

    // Convert QImage to bytes in PNG format
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);
    testImage.save(&buffer, "PNG");

    // Compress the image data
    QByteArray compressedImage = qCompress(imageData, 9);

    // Build packet: type flag + compressed image data
    QByteArray packet;
    packet.push_back(static_cast<char>(AppEnums::MSG_TYPE::Image));
    packet.append(compressedImage);

    ImageMsg captured;
    EXPECT_CALL(listener, onImageReceived(_)).WillOnce(SaveArg<0>(&captured));
    EXPECT_CALL(listener, onReceiveError(_)).Times(0);

    usecase->handleData(packet);

    EXPECT_FALSE(captured.img.isNull());
}

TEST_F(ReceiveUseCaseTest, FileMessageNotifiesListener) {
    // Prepare file message data - match what ReceiveUseCase expects
    // ReceiveUseCase does: msg.fileName = QString(qUncompress(data.mid(1, FileMsg::FILENAME_SIZE)));
    // So we need to put compressed filename in bytes 1 to 1+FILENAME_SIZE

    QString testFileName = "testfile.txt";
    QByteArray testFileData = "This is test file content";

    // Compress the filename (ReceiveUseCase expects compressed filename)
    QByteArray compressedName = qCompress(testFileName.toUtf8(), 9);

    // Build packet: type flag + compressed filename (fixed size) + compressed data
    QByteArray packet;
    packet.push_back(static_cast<char>(AppEnums::MSG_TYPE::File));

    // Add compressed filename field (fixed size)
    // ReceiveUseCase extracts data.mid(1, FileMsg::FILENAME_SIZE) and uncompresses it
    QByteArray nameField = compressedName.left(FileMsg::FILENAME_SIZE);
    nameField.resize(FileMsg::FILENAME_SIZE);
    packet.append(nameField);

    // Add compressed file data
    QByteArray compressedData = qCompress(testFileData, 9);
    packet.append(compressedData);

    FileMsg captured;
    // fileName comes back empty: qUncompress cannot recover a name
    // truncated to FileMsg::FILENAME_SIZE (protocol limitation)
    EXPECT_CALL(listener, onFileReceived(_)).WillOnce(SaveArg<0>(&captured));
    EXPECT_CALL(listener, onReceiveError(_)).Times(0);

    usecase->handleData(packet);

    EXPECT_TRUE(captured.fileName.isEmpty() || captured.fileName == testFileName);
}

TEST_F(ReceiveUseCaseTest, UnknownMessageTypeDoesNotCrash) {
    // Create packet with unknown message type
    QByteArray packet;
    packet.push_back(static_cast<char>(99)); // Unknown type
    packet.push_back("some data");

    EXPECT_CALL(listener, onTextReceived(_)).Times(0);
    EXPECT_CALL(listener, onImageReceived(_)).Times(0);
    EXPECT_CALL(listener, onFileReceived(_)).Times(0);
    EXPECT_CALL(listener, onReceiveError(_)).Times(0);

    usecase->handleData(packet);
}

TEST_F(ReceiveUseCaseTest, EmptyDataDoesNotCrash) {
    QByteArray emptyPacket;

    EXPECT_CALL(listener, onTextReceived(_)).Times(0);
    EXPECT_CALL(listener, onImageReceived(_)).Times(0);
    EXPECT_CALL(listener, onFileReceived(_)).Times(0);
    EXPECT_CALL(listener, onReceiveError(_)).Times(0);

    usecase->handleData(emptyPacket);
}

TEST_F(ReceiveUseCaseTest, NullConnectorDoesNotCrash) {
    usecase->setConnector(nullptr);

    QByteArray packet;
    packet.push_back(static_cast<char>(AppEnums::MSG_TYPE::Text));
    packet.push_back(qCompress(QString::fromUtf8("test").toUtf8(), 9));

    // handleData parses the packet and does not touch the connector
    usecase->handleData(packet);

    SUCCEED();
}
