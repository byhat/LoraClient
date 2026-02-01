<div align="center">

# LoRaClient

### Qt6/C++/QML клиент для работы с LoRa-модулем Ebyte E22-400T22U

[![Build](https://github.com/byhat/LoraClient/actions/workflows/build-linux.yml/badge.svg)](https://github.com/byhat/LoraClient/actions/workflows/build-linux.yml)
[![License](https://img.shields.io/github/license/byhat/LoraClient)](LICENSE)
[![Qt](https://img.shields.io/badge/Qt-6.0+-41CD52?logo=qt)](https://www.qt.io/)
[![C++](https://img.shields.io/badge/C++-20-00599C?logo=cplusplus)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.19+-064F8C?logo=cmake)](https://cmake.org/)

[Watch the demo](doc/video/demo.mp4) | [Report Bug](https://github.com/byhat/LoraClient/issues) | [Request Feature](https://github.com/byhat/LoraClient/issues)

</div>

---

## 📋 Описание проекта

**LoRaClient** — это современное десктопное приложение для отправки и получения данных через LoRa-модуль Ebyte E22-400T22U. Приложение построено с использованием Clean Architecture и предоставляет удобный графический интерфейс на базе QML.

### 🌟 Основные возможности

- 📤 **Отправка сообщений** — текстовых, изображений и файлов
- 📥 **Прием данных** — автоматическая обработка входящих сообщений
- 🔄 **Фрагментация** — автоматическое разбиение больших пакетов
- ✅ **Подтверждение доставки** — ACK на уровне фрагмента и пакета
- 🎨 **Современный UI** — интерфейс на QML с поддержкой темной темы
- 🧪 **Покрытие тестами** — unit-тесты для всех use cases
- 🏗️ **Clean Architecture** — разделение на слои domain, application, infrastructure, presentation

---

## 🎸 Демо

[![Watch the video](./doc/images/screen.png)](./doc/video/demo.mp4)

---

## 🚀 Быстрый старт

### Требования

- **CMake** ≥ 3.19
- **Qt6** (Core, Qml, Gui, SerialPort, Concurrent, Test, Gui)
- **Компилятор** с поддержкой C++20 (GCC 10+, Clang 12+, MSVC 2019+)
- **spdlog** — библиотека логирования
- **GTest/GMock** — фреймворки для тестирования

### Установка зависимостей

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-declarative-dev \
    qt6-serialport-dev \
    libqt6svg6-dev \
    libspdlog-dev \
    libgtest-dev \
    libgmock-dev \
    qml6-module-qtquick \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts
```

#### Fedora

```bash
sudo dnf install -y \
    cmake \
    qt6-qtbase-devel \
    qt6-qtdeclarative-devel \
    qt6-qtserialport-devel \
    spdlog-devel \
    gtest-devel \
    gmock-devel
```

#### Arch Linux

```bash
sudo pacman -S --needed \
    cmake \
    qt6-base \
    qt6-declarative \
    qt6-serialport \
    spdlog \
    gtest
```

### Сборка

```bash
# Клонируем репозиторий с подмодулями
git clone --recurse-submodules https://github.com/byhat/LoraClient.git
cd LoraClient

# Создаем директорию для сборки
mkdir -p build && cd build

# Конфигурируем проект
cmake .. -DCMAKE_BUILD_TYPE=Release

# Собираем проект
cmake --build . --parallel

# Запускаем
./LoraClient
```

### Запуск тестов

```bash
cd build
./tests/loraclient_tests
```

---

## 📁 Структура проекта

```
LoraClient/
├── LoraClient/              # Основное приложение
│   ├── src/
│   │   ├── application/     # Application layer (AppEngine)
│   │   ├── core/            # Business logic
│   │   │   ├── entities/    # Сущности предметной области
│   │   │   └── usecases/    # Use cases (Connection, Send, Receive)
│   │   ├── domain/          # Domain layer (interfaces)
│   │   ├── infrastructure/  # Infrastructure layer (gateways, logging)
│   │   └── presentation/    # Presentation layer (controllers, models, QML)
│   ├── qml/                 # QML интерфейсы
│   └── resources/           # Ресурсы (шрифты, иконки)
├── tests/                   # Unit-тесты
├── tools/                   # Дополнительные утилиты
│   └── LoraEchoServer/      # Echo-сервер для тестирования
└── doc/                     # Документация
```

---

## 🏗️ Архитектура

Проект построен по принципам **Clean Architecture**:

- **Domain Layer** — содержит бизнес-логику и интерфейсы
- **Application Layer** — оркестрация use cases
- **Infrastructure Layer** — реализация внешних зависимостей (LoRa, логирование)
- **Presentation Layer** — UI на QML и контроллеры

### Use Cases

- **ConnectionUseCase** — управление подключением к LoRa-модулю
- **SendUseCase** — отправка текста, изображений и файлов
- **ReceiveUseCase** — прием и обработка входящих данных

---

## 📡 Протокол

| Параметр | Значение |
|----------|---------|
| Макс. размер фрейма | 26 байт данных + заголовок |
| Фрагментация | Автоматическая для больших пакетов |
| Подтверждение | ACK (фрагмент) + PACKET_ACK (пакет) |
| Повторы | До 5 раз при потере ACK |
| Таймаут | 1 секунда |
| Контрольная сумма | CRC8 (Dallas/Maxim) |

---

## 🧪 Тестирование

Проект покрыт unit-тестами с использованием Google Test:

```bash
# Запуск всех тестов
cd build
./tests/loraclient_tests

# Запуск с выводом подробной информации
./tests/loraclient_tests --gtest_verbose

# Запуск конкретного теста
./tests/loraclient_tests --gtest_filter="SendUseCaseTest.SendTextCallsSendPacket"
```

### Покрытие тестами

- ✅ ConnectionUseCase — 4 теста
- ✅ ReceiveUseCase — 6 тестов
- ✅ SendUseCase — 10 тестов

---

## 🔧 Echo-сервер (опционально)

Для тестирования можно собрать Echo-сервер:

```bash
cd tools/LoraEchoServer
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel

# Запуск
./LoraCmd
```

---

## 🤝 Вклад в проект

Мы приветствуем вклад в развитие проекта! Пожалуйста, следуйте этим правилам:

1. Создайте fork проекта
2. Создайте ветку для вашей функции (`git checkout -b feature/AmazingFeature`)
3. Закоммитьте изменения (`git commit -m 'Add some AmazingFeature'`)
4. Запушьте в ветку (`git push origin feature/AmazingFeature`)
5. Откройте Pull Request

---

## 📝 Лицензия

Этот проект распространяется под лицензией MIT — см. файл [LICENSE](LICENSE) для подробностей.

---

## 👏 Благодарности

- **Qt Framework** — отличный фреймворк для кроссплатформенной разработки
- **spdlog** — быстрая C++ библиотека логирования
- **Google Test** — фреймворк для тестирования C++ кода
- **Ebyte** — за качественные LoRa-модули

---

## 📞 Контакты

**byhat** — [GitHub](https://github.com/byhat)

Ссылка на проект: [https://github.com/byhat/LoraClient](https://github.com/byhat/LoraClient)

---

<div align="center">

**⭐ Если этот проект вам понравился, поставьте звезду! ⭐**

</div>
