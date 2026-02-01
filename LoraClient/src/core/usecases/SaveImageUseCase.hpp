#pragma once

#include <mutex>

#include <QObject>
#include <QString>
#include <QImage>

#include <src/infrastructure/loggining/ILogger.hpp>

class SaveImageUseCase : public QObject {
    Q_OBJECT

public:
    explicit SaveImageUseCase(QObject *parent = nullptr);

    void setLogger(infrastructure::ILoggerPtr logger);

public slots:
    void saveImage(const QString &base64Data, const QString &timestamp);
    void saveImageToPath(const QString &base64Data, const QString &timestamp, const QString &filePath);

signals:
    void imageSaved(const QString &filePath);
    void errorOccured(const QString &errorMessage);

private:
    bool saveImageToFile(const QImage &image, const QString &timestamp, QString &outFilePath);
    bool saveImageToSpecificPath(const QImage &image, const QString &filePath);

    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex;
};
