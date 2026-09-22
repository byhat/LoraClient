#include <gtest/gtest.h>
#include <QBuffer>
#include <QImage>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>

#include "../LoraClient/src/core/usecases/SaveImageUseCase.hpp"
#include "MockListeners.hpp"

using namespace testing;

class SaveImageUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<SaveImageUseCase>();
        usecase->setListener(&listener);
    }
    std::unique_ptr<SaveImageUseCase> usecase;
    MockSaveImageListener listener;

    // Helper function to create a base64 encoded test image
    QString createTestImageBase64(QColor color = Qt::red, int width = 100, int height = 100) {
        QImage testImage(width, height, QImage::Format_RGB32);
        testImage.fill(color);

        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        testImage.save(&buffer, "PNG");

        return "data:image/png;base64," + imageData.toBase64();
    }

    // Helper function to create a base64 encoded test image without prefix
    QString createTestImageBase64NoPrefix(QColor color = Qt::blue, int width = 50, int height = 50) {
        QImage testImage(width, height, QImage::Format_RGB32);
        testImage.fill(color);

        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        testImage.save(&buffer, "PNG");

        return imageData.toBase64();
    }

    void cleanupSavedFile(const QString &filePath) {
        QFile::remove(filePath);
        QDir dir = QFileInfo(filePath).absoluteDir();
        if (dir.dirName() == "LoraClient") {
            dir.removeRecursively();
        }
    }
};

TEST_F(SaveImageUseCaseTest, SaveImageWithValidBase64Data) {
    QString base64Data = createTestImageBase64();
    QString timestamp = "12:30:45";

    QString filePath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&filePath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImage(base64Data, timestamp);

    // Verify the file path in the notification
    EXPECT_TRUE(filePath.contains("LoraClient"));
    EXPECT_TRUE(filePath.endsWith(".png"));

    // Verify the file was actually created
    EXPECT_TRUE(QFile::exists(filePath));

    cleanupSavedFile(filePath);
}

TEST_F(SaveImageUseCaseTest, SaveImageWithBase64DataNoPrefix) {
    QString base64Data = createTestImageBase64NoPrefix();
    QString timestamp = "15:20:30";

    QString filePath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&filePath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImage(base64Data, timestamp);

    cleanupSavedFile(filePath);
}

TEST_F(SaveImageUseCaseTest, SaveImageWithEmptyBase64Data) {
    QString emptyData = "";
    QString timestamp = "10:00:00";

    QString errorMsg;
    EXPECT_CALL(listener, onImageSaved(_)).Times(0);
    EXPECT_CALL(listener, onSaveImageError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->saveImage(emptyData, timestamp);

    EXPECT_TRUE(errorMsg.contains("base64 data is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithInvalidBase64Data) {
    QString invalidData = "this_is_not_valid_base64_data!!!";
    QString timestamp = "11:11:11";

    QString errorMsg;
    EXPECT_CALL(listener, onImageSaved(_)).Times(0);
    EXPECT_CALL(listener, onSaveImageError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->saveImage(invalidData, timestamp);

    EXPECT_TRUE(errorMsg.contains("failed to decode base64") ||
                errorMsg.contains("failed to load image"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithEmptyTimestamp) {
    QString base64Data = createTestImageBase64();
    QString emptyTimestamp = "";

    QString filePath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&filePath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImage(base64Data, emptyTimestamp);

    cleanupSavedFile(filePath);
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPath) {
    QString base64Data = createTestImageBase64(Qt::green);
    QString timestamp = "14:25:36";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("test_image.png");

    QString savedPath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&savedPath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    // Verify the file was created at the specified path
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify the image can be loaded
    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
    EXPECT_EQ(savedImage.width(), 100);
    EXPECT_EQ(savedImage.height(), 100);
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithJpgExtension) {
    QString base64Data = createTestImageBase64(Qt::yellow);
    QString timestamp = "16:45:00";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("test_image.jpg");

    EXPECT_CALL(listener, onImageSaved(_)).Times(1);
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_TRUE(QFile::exists(filePath));

    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithBmpExtension) {
    QString base64Data = createTestImageBase64(Qt::cyan);
    QString timestamp = "18:30:15";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("test_image.bmp");

    EXPECT_CALL(listener, onImageSaved(_)).Times(1);
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_TRUE(QFile::exists(filePath));

    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathCreatesDirectory) {
    QString base64Data = createTestImageBase64(Qt::magenta);
    QString timestamp = "20:10:05";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString nestedDir = tempDir.filePath("nested/directory/structure");
    QString filePath = nestedDir + "/test_image.png";

    EXPECT_CALL(listener, onImageSaved(_)).Times(1);
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_TRUE(QFile::exists(filePath));

    QDir(tempDir.path()).removeRecursively();
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithEmptyBase64) {
    QString emptyData = "";
    QString timestamp = "21:00:00";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("test_image.png");

    QString errorMsg;
    EXPECT_CALL(listener, onImageSaved(_)).Times(0);
    EXPECT_CALL(listener, onSaveImageError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->saveImageToPath(emptyData, timestamp, filePath);

    EXPECT_TRUE(errorMsg.contains("base64 data is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithEmptyFilePath) {
    QString base64Data = createTestImageBase64();
    QString timestamp = "22:30:00";
    QString emptyFilePath = "";

    QString errorMsg;
    EXPECT_CALL(listener, onImageSaved(_)).Times(0);
    EXPECT_CALL(listener, onSaveImageError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->saveImageToPath(base64Data, timestamp, emptyFilePath);

    EXPECT_TRUE(errorMsg.contains("file path is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithInvalidBase64) {
    QString invalidData = "invalid_base64_data";
    QString timestamp = "23:45:00";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("test_image.png");

    QString errorMsg;
    EXPECT_CALL(listener, onImageSaved(_)).Times(0);
    EXPECT_CALL(listener, onSaveImageError(_)).WillOnce(SaveArg<0>(&errorMsg));

    usecase->saveImageToPath(invalidData, timestamp, filePath);

    EXPECT_TRUE(errorMsg.contains("failed to decode base64") ||
                errorMsg.contains("failed to load image"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithInvalidTimestampFormat) {
    QString base64Data = createTestImageBase64();
    QString invalidTimestamp = "invalid_time";

    QString filePath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&filePath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImage(base64Data, invalidTimestamp);

    cleanupSavedFile(filePath);
}

TEST_F(SaveImageUseCaseTest, SaveImageWithValidTimestampFormat) {
    QString base64Data = createTestImageBase64();
    QString validTimestamp = "08:30:45";

    QString filePath;
    EXPECT_CALL(listener, onImageSaved(_)).WillOnce(SaveArg<0>(&filePath));
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImage(base64Data, validTimestamp);

    // Verify the filename contains the time
    EXPECT_TRUE(filePath.contains("08-30-45"));

    cleanupSavedFile(filePath);
}

TEST_F(SaveImageUseCaseTest, SaveImageToPathWithNoPrefixBase64) {
    QString base64Data = createTestImageBase64NoPrefix(Qt::darkRed);
    QString timestamp = "05:15:20";

    QTemporaryDir tempDir;
    ASSERT_TRUE(tempDir.isValid());

    QString filePath = tempDir.filePath("no_prefix_image.png");

    EXPECT_CALL(listener, onImageSaved(_)).Times(1);
    EXPECT_CALL(listener, onSaveImageError(_)).Times(0);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_TRUE(QFile::exists(filePath));

    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}
