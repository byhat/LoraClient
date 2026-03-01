cmake_minimum_required(VERSION 3.16)

project(LoraClient VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Qml Gui Concurrent REQUIRED)
# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/android")
    set(QT_ANDROID_TARGET_SDK_VERSION "34")
    # Explicitly set Android SDK versions to avoid Qt defaulting to 35
    set(CMAKE_ANDROID_API 34)
    set(CMAKE_ANDROID_BUILD_TOOLS_VERSION "34.0.0")
    
    # Copy Java files from QCrossPlatformSerial library to android/src
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtcrossplatformserial/UsbSerialManager.java
        DESTINATION android/src/org/qtcrossplatformserial/
    )
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtcrossplatformserial/JniUsbSerial.java
        DESTINATION android/src/org/qtcrossplatformserial/
    )
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtcrossplatformserial/SerialInputOutputManager.java
        DESTINATION android/src/org/qtcrossplatformserial/
    )
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtserial/UsbSerialManager.java
        DESTINATION android/src/org/qtserial/
    )
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtserial/JniUsbSerial.java
        DESTINATION android/src/org/qtserial/
    )
    file(COPY
        LoraClient/src/infrastructure/gateway/LoRaCore/thirdparty/QCrossPlatformSerial/android/src/org/qtserial/SerialInputOutputManager.java
        DESTINATION android/src/org/qtserial/
    )
endif()

add_subdirectory(LoraClient)

if(NOT ANDROID)
    enable_testing()
    add_subdirectory(tests)
endif()

# documentation
file(GLOB DOC_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.qmodel"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.md"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.drawio"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/README*"
)

add_custom_target(doc_files SOURCES ${DOC_FILES})
