#include <QImage>
#include <QString>
#include <QDateTime>

/**
 * @brief Структура для передачи изображения с временной меткой
 */
struct ImageMsg {
    QImage img;          //< Изображение
    QDateTime time;      //< Временная метка
};

/**
 * @brief Структура для передачи текстового сообщения с временной меткой
 */
struct TextMsg {
    QString text;        //< Текстовое содержимое сообщения
    QDateTime time;      //< Временная метка
};

/**
 * @brief Структура для передачи информации о файле с временной меткой
 */
struct FileMsg {
    QString fileName;    //< Имя файла
    QDateTime time;      //< Временная метка
};
