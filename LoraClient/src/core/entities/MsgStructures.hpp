#pragma once

#include <QImage>
#include <QString>
#include <QDateTime>

class AppEnums : public QObject
{
    Q_OBJECT

public:
    static constexpr int MSG_TYPE_FLAG_SIZE = 1;
    enum MSG_TYPE {
        Text,
        Image,
        File
    };
    Q_ENUM(MSG_TYPE)

    AppEnums() = delete;
};

/**
 * @brief Структура для передачи изображения с временной меткой
 */
struct ImageMsg {
    QImage img;          //!< Изображение
    QDateTime time;      //!< Временная метка
};

/**
 * @brief Структура для передачи текстового сообщения с временной меткой
 */
struct TextMsg {
    QString text;        //!< Текстовое содержимое сообщения
    QDateTime time;      //!< Временная метка
};

/**
 * @brief Структура для передачи информации о файле с временной меткой
 */
struct FileMsg {
    static constexpr int FILENAME_SIZE = 15;
    QString fileName;    //!< Имя файла
    QByteArray data;     //!< Содержимое файла
    QDateTime time;      //!< Временная метка
};
