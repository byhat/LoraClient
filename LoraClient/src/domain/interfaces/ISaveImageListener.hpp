#pragma once

#include <QString>

class ISaveImageListener {
public:
    virtual ~ISaveImageListener() = default;

    virtual void onImageSaved(const QString &filePath) = 0;
    virtual void onSaveImageError(const QString &error) = 0;
};
