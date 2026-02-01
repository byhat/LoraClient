#include "SaveImageUseCase.hpp"

#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QByteArray>
#include <QBuffer>
#include <QDateTime>
#include <QDebug>

SaveImageUseCase::SaveImageUseCase(QObject *parent)
    : QObject{parent}
{
}

void SaveImageUseCase::setLogger(infrastructure::ILoggerPtr logger)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_logger = logger;
}

void SaveImageUseCase::saveImage(const QString &base64Data, const QString &timestamp)
{
    if (base64Data.isEmpty()) {
        QString error = "Cannot save image: base64 data is empty";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Remove data URI prefix if present
    QString processedData = base64Data;
    const QString prefix = "data:image/png;base64,";
    if (processedData.startsWith(prefix)) {
        processedData = processedData.mid(prefix.length());
    }

    // Decode base64 data
    QByteArray imageData = QByteArray::fromBase64(processedData.toUtf8());
    if (imageData.isEmpty()) {
        QString error = "Cannot save image: failed to decode base64 data";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Load image from decoded data
    QImage image;
    if (!image.loadFromData(imageData)) {
        QString error = "Cannot save image: failed to load image from decoded data";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    if (image.isNull()) {
        QString error = "Cannot save image: loaded image is null";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Save image to file
    QString filePath;
    if (saveImageToFile(image, timestamp, filePath)) {
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Info, "Image saved successfully to: " + filePath.toStdString());
        }
        emit imageSaved(filePath);
    } else {
        QString error = "Failed to save image to file";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
    }
}

void SaveImageUseCase::saveImageToPath(const QString &base64Data, const QString &timestamp, const QString &filePath)
{
    if (base64Data.isEmpty()) {
        QString error = "Cannot save image: base64 data is empty";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    if (filePath.isEmpty()) {
        QString error = "Cannot save image: file path is empty";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Remove data URI prefix if present
    QString processedData = base64Data;
    const QString prefix = "data:image/png;base64,";
    if (processedData.startsWith(prefix)) {
        processedData = processedData.mid(prefix.length());
    }

    // Decode base64 data
    QByteArray imageData = QByteArray::fromBase64(processedData.toUtf8());
    if (imageData.isEmpty()) {
        QString error = "Cannot save image: failed to decode base64 data";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Load image from decoded data
    QImage image;
    if (!image.loadFromData(imageData)) {
        QString error = "Cannot save image: failed to load image from decoded data";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    if (image.isNull()) {
        QString error = "Cannot save image: loaded image is null";
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
        return;
    }

    // Save image to the specified path
    if (saveImageToSpecificPath(image, filePath)) {
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Info, "Image saved successfully to: " + filePath.toStdString());
        }
        emit imageSaved(filePath);
    } else {
        QString error = "Failed to save image to file: " + filePath;
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, error.toStdString());
        }
        emit errorOccured(error);
    }
}

bool SaveImageUseCase::saveImageToFile(const QImage &image, const QString &timestamp, QString &outFilePath)
{
    // Get pictures directory
    QString picturesDir = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation);
    if (picturesDir.isEmpty()) {
        if (m_logger) {
            m_logger->log(infrastructure::LogLevel::Error, "Failed to get pictures directory");
        }
        return false;
    }

    // Create subdirectory for saved images
    QString saveDir = picturesDir + "/LoraClient";
    QDir dir(saveDir);
    if (!dir.exists()) {
        if (!dir.mkpath(saveDir)) {
            if (m_logger) {
                m_logger->log(infrastructure::LogLevel::Error, "Failed to create directory: " + saveDir.toStdString());
            }
            return false;
        }
    }

    // Generate filename using timestamp
    QString filename;
    if (!timestamp.isEmpty()) {
        // Parse timestamp to create a valid filename
        QDateTime dateTime = QDateTime::fromString(timestamp, "HH:mm:ss");
        if (dateTime.isValid()) {
            // Use current date with provided time
            QDateTime now = QDateTime::currentDateTime();
            dateTime.setDate(now.date());
            filename = "image_" + dateTime.toString("yyyy-MM-dd_HH-mm-ss") + ".png";
        } else {
            // Fallback to current datetime
            filename = "image_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".png";
        }
    } else {
        filename = "image_" + QDateTime::currentDateTime().toString("yyyy-MM-dd_HH-mm-ss") + ".png";
    }

    QString fullPath = saveDir + "/" + filename;

    // Save image
    if (image.save(fullPath, "PNG")) {
        outFilePath = fullPath;
        return true;
    }

    if (m_logger) {
        m_logger->log(infrastructure::LogLevel::Error, "Failed to save image to: " + fullPath.toStdString());
    }
    return false;
}

bool SaveImageUseCase::saveImageToSpecificPath(const QImage &image, const QString &filePath)
{
    // Ensure the directory exists
    QFileInfo fileInfo(filePath);
    QString dirPath = fileInfo.absolutePath();
    QDir dir(dirPath);
    
    if (!dir.exists()) {
        if (!dir.mkpath(dirPath)) {
            if (m_logger) {
                m_logger->log(infrastructure::LogLevel::Error, "Failed to create directory: " + dirPath.toStdString());
            }
            return false;
        }
    }

    // Determine the format from the file extension
    QString format = "PNG";
    QString suffix = fileInfo.suffix().toLower();
    if (suffix == "jpg" || suffix == "jpeg") {
        format = "JPG";
    } else if (suffix == "bmp") {
        format = "BMP";
    }

    // Save image
    if (image.save(filePath, format.toUtf8().constData())) {
        return true;
    }

    if (m_logger) {
        m_logger->log(infrastructure::LogLevel::Error, "Failed to save image to: " + filePath.toStdString());
    }
    return false;
}
