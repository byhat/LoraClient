#include <QObject>
#include <QString>
#include <QByteArray>


/**
 * @brief Интерфейс для работы с каналом связи (например, COM-порт, UDP, TCP и т.д.)
 *
 * Предназначен для инверсии зависимостей: позволяет бизнес-логике взаимодействовать с различными классами связи через единый API, не завися от их реализации. Все методы являются чисто виртуальными и должны быть реализованы в дочерних классах
 */
class IConnectionWorker : public QObject
{
    Q_OBJECT

public:
    virtual ~IConnectionWorker() = default;

    /**
     * @brief Открывает соединение с удалённой стороной или локальным ресурсом
     *
     * Поведение зависит от реализации, для последовательного порта: открывает последовательный порт с указанными параметрами
     * @param portName Имя порта/адрес ресурса (например, "/dev/ttyUSB0", "192.168.1.1:5502").
     * @param baud Скорость передачи (актуально для последовательного порта; для сетевых транспортов может игнорироваться).
     */
    virtual void openPort(const QString &portName, qint32 baud = 9600) = 0;

    /**
     * @brief Закрывает активное соединение
     */
    virtual void closePort() noexcept = 0;

    /**
     * @brief Отправляет пакет данных через установленное соединение
     * @param data Байтовый массив, содержащий данные для отправки
     */
    virtual void sendPacket(const QByteArray &data) = 0;

signals:
    /**
     * @brief Сигнализирует об успешности/неудаче открытия порта
     *
     * @param ok @c true, если порт открыт и готов к работе; @c false - в случае ошибки.
     * @param error Описание ошибки
     */
    void portOpened(bool ok, const QString &error = {});

    /**
     * @brief Указывает, успешно ли был отправлен пакет
     *
     * @param success @c true, если все байты доставлены; @c false — при ошибке
     */
    void packetSent(bool success);

    /**
     * @brief Сигнализирует о прогрессе отправки пакета
     *
     * @param sentBytes Количество байт, уже отправленных
     * @param totalBytes Общий размер пакета
     */
    void packetSendProgress(int sentBytes, int totalBytes);

    /**
     * @brief Сигнализирует о получении полного пакета данных
     *
     * @param data Полученные данные
     */
    void packetReceived(const QByteArray &data);

    /**
     * @brief Сигнализирует о прогрессе приёма пакета
     *
     * @param receivedBytes Количество байт, принятых на данный момент для текущего пакета
     * @param totalBytes Ожидаемый размер пакета
     */
    void packetReceiveProgress(int receivedBytes, int totalBytes);

    /**
     * @brief Сигнализирует об ошибке в работе соединения
     *
     * @param msg Человекочитаемое описание ошибки
     */
    void errorOccurred(const QString &msg);
};
