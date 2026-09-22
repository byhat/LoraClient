#pragma once

#include <mutex>

#include <QString>
#include <QImage>

#include <src/domain/interfaces/ISaveImageListener.hpp>
#include <src/infrastructure/loggining/ILogger.hpp>

class SaveImageUseCase {
public:
    SaveImageUseCase() = default;

    void setListener(ISaveImageListener *listener);
    void setLogger(infrastructure::ILoggerPtr logger);

    void saveImage(const QString &base64Data, const QString &timestamp);
    void saveImageToPath(const QString &base64Data, const QString &timestamp, const QString &filePath);

private:
    bool saveImageToFile(const QImage &image, const QString &timestamp, QString &outFilePath);
    bool saveImageToSpecificPath(const QImage &image, const QString &filePath);

    void notifyError(const QString &error);

    ISaveImageListener *m_listener = nullptr;
    infrastructure::ILoggerPtr m_logger;
    std::mutex m_mutex;
};
