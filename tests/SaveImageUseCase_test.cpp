#include <gtest/gtest.h>
#include <QtTest/QSignalSpy>
#include <QBuffer>
#include <QImage>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>

#include "../LoraClient/src/core/usecases/SaveImageUseCase.hpp"

using namespace testing;

class SaveImageUseCaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        usecase = std::make_unique<SaveImageUseCase>();
    }
    std::unique_ptr<SaveImageUseCase> usecase;

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
};

TEST_F(SaveImageUseCaseTest, SaveImageWithValidBase64Data) {
    QString base64Data = createTestImageBase64();
    QString timestamp = "12:30:45";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(base64Data, timestamp);

    // Should emit imageSaved signal
    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the file path in the signal
    QList<QVariant> arguments = savedSpy.takeFirst();
    QString filePath = arguments.at(0).toString();
    EXPECT_TRUE(filePath.contains("LoraClient"));
    EXPECT_TRUE(filePath.endsWith(".png"));

    // Verify the file was actually created
    EXPECT_TRUE(QFile::exists(filePath));

    // Clean up
    QFile::remove(filePath);
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (dir.dirName() == "LoraClient") {
        dir.removeRecursively();
    }
}

TEST_F(SaveImageUseCaseTest, SaveImageWithBase64DataNoPrefix) {
    QString base64Data = createTestImageBase64NoPrefix();
    QString timestamp = "15:20:30";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(base64Data, timestamp);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Clean up
    QList<QVariant> arguments = savedSpy.takeFirst();
    QString filePath = arguments.at(0).toString();
    QFile::remove(filePath);
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (dir.dirName() == "LoraClient") {
        dir.removeRecursively();
    }
}

TEST_F(SaveImageUseCaseTest, SaveImageWithEmptyBase64Data) {
    QString emptyData = "";
    QString timestamp = "10:00:00";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(emptyData, timestamp);

    EXPECT_EQ(savedSpy.count(), 0);
    EXPECT_EQ(errorSpy.count(), 1);

    QList<QVariant> arguments = errorSpy.takeFirst();
    QString errorMsg = arguments.at(0).toString();
    EXPECT_TRUE(errorMsg.contains("base64 data is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithInvalidBase64Data) {
    QString invalidData = "this_is_not_valid_base64_data!!!";
    QString timestamp = "11:11:11";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(invalidData, timestamp);

    EXPECT_EQ(savedSpy.count(), 0);
    EXPECT_EQ(errorSpy.count(), 1);

    QList<QVariant> arguments = errorSpy.takeFirst();
    QString errorMsg = arguments.at(0).toString();
    EXPECT_TRUE(errorMsg.contains("failed to decode base64") || 
                errorMsg.contains("failed to load image"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithEmptyTimestamp) {
    QString base64Data = createTestImageBase64();
    QString emptyTimestamp = "";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(base64Data, emptyTimestamp);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Clean up
    QList<QVariant> arguments = savedSpy.takeFirst();
    QString filePath = arguments.at(0).toString();
    QFile::remove(filePath);
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (dir.dirName() == "LoraClient") {
        dir.removeRecursively();
    }
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPath) {
    QString base64Data = createTestImageBase64(Qt::green);
    QString timestamp = "14:25:36";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("test_image.png");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

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
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("test_image.jpg");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the file was created
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify the image can be loaded
    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithBmpExtension) {
    QString base64Data = createTestImageBase64(Qt::cyan);
    QString timestamp = "18:30:15";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("test_image.bmp");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the file was created
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify the image can be loaded
    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathCreatesDirectory) {
    QString base64Data = createTestImageBase64(Qt::magenta);
    QString timestamp = "20:10:05";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString nestedDir = tempDir.filePath("nested/directory/structure");
    QString filePath = nestedDir + "/test_image.png";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the file was created
    EXPECT_TRUE(QFile::exists(filePath));

    // Clean up
    QDir(tempDir.path()).removeRecursively();
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithEmptyBase64) {
    QString emptyData = "";
    QString timestamp = "21:00:00";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("test_image.png");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(emptyData, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 0);
    EXPECT_EQ(errorSpy.count(), 1);

    QList<QVariant> arguments = errorSpy.takeFirst();
    QString errorMsg = arguments.at(0).toString();
    EXPECT_TRUE(errorMsg.contains("base64 data is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithEmptyFilePath) {
    QString base64Data = createTestImageBase64();
    QString timestamp = "22:30:00";
    QString emptyFilePath = "";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, emptyFilePath);

    EXPECT_EQ(savedSpy.count(), 0);
    EXPECT_EQ(errorSpy.count(), 1);

    QList<QVariant> arguments = errorSpy.takeFirst();
    QString errorMsg = arguments.at(0).toString();
    EXPECT_TRUE(errorMsg.contains("file path is empty"));
}

TEST_F(SaveImageUseCaseTest, SaveImageToSpecificPathWithInvalidBase64) {
    QString invalidData = "invalid_base64_data";
    QString timestamp = "23:45:00";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("test_image.png");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(invalidData, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 0);
    EXPECT_EQ(errorSpy.count(), 1);

    QList<QVariant> arguments = errorSpy.takeFirst();
    QString errorMsg = arguments.at(0).toString();
    EXPECT_TRUE(errorMsg.contains("failed to decode base64") || 
                errorMsg.contains("failed to load image"));
}

TEST_F(SaveImageUseCaseTest, SaveImageWithInvalidTimestampFormat) {
    QString base64Data = createTestImageBase64();
    QString invalidTimestamp = "invalid_time";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(base64Data, invalidTimestamp);

    // Should still save, but with current datetime
    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Clean up
    QList<QVariant> arguments = savedSpy.takeFirst();
    QString filePath = arguments.at(0).toString();
    QFile::remove(filePath);
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (dir.dirName() == "LoraClient") {
        dir.removeRecursively();
    }
}

TEST_F(SaveImageUseCaseTest, SaveImageWithValidTimestampFormat) {
    QString base64Data = createTestImageBase64();
    QString validTimestamp = "08:30:45";

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImage(base64Data, validTimestamp);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the filename contains the time
    QList<QVariant> arguments = savedSpy.takeFirst();
    QString filePath = arguments.at(0).toString();
    EXPECT_TRUE(filePath.contains("08-30-45"));

    // Clean up
    QFile::remove(filePath);
    QDir dir = QFileInfo(filePath).absoluteDir();
    if (dir.dirName() == "LoraClient") {
        dir.removeRecursively();
    }
}

TEST_F(SaveImageUseCaseTest, SaveImageToPathWithNoPrefixBase64) {
    QString base64Data = createTestImageBase64NoPrefix(Qt::darkRed);
    QString timestamp = "05:15:20";
    
    QTemporaryDir tempDir;
    QVERIFY(tempDir.isValid());
    
    QString filePath = tempDir.filePath("no_prefix_image.png");

    QSignalSpy savedSpy(usecase.get(), &SaveImageUseCase::imageSaved);
    QSignalSpy errorSpy(usecase.get(), &SaveImageUseCase::errorOccured);

    usecase->saveImageToPath(base64Data, timestamp, filePath);

    EXPECT_EQ(savedSpy.count(), 1);
    EXPECT_EQ(errorSpy.count(), 0);

    // Verify the file was created
    EXPECT_TRUE(QFile::exists(filePath));

    // Verify the image can be loaded
    QImage savedImage(filePath);
    EXPECT_FALSE(savedImage.isNull());
}
