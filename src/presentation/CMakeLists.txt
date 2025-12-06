cmake_minimum_required(VERSION 3.16)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Qml Gui SerialPort Concurrent REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    AppEngine.hpp
    AppEngine.cpp

    common/AsyncLogger.hpp
    common/ConfigReader.hpp
    common/MessagesHandler.hpp
    common/structures.hpp
    common/FileHelper.hpp
    common/QMsgHandler.hpp

    gateway/LoraAdapter.hpp
    gateway/LoraAdapter.cpp
    gateway/LoraStructures.hpp
    gateway/lora_reliable_transport.h
    gateway/lora_reliable_transport.cpp
    gateway/loraworker.h
    gateway/loraworker.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
add_definitions(-lwiringPi -lpthread)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI AppQml
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        qml/ConnectionPage.qml
        qml/ChatPage.qml

    RESOURCES
        icons/chat.svg
        icons/setting.svg
        # icons/info.svg
        # icons/moon.svg
        # icons/sun.svg
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt6::Core
        Qt6::Qml
        Qt6::Gui
        Qt6::SerialPort
        Qt6::Concurrent
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
