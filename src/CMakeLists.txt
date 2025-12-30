cmake_minimum_required(VERSION 3.16)

option(LET_IT_SNOW "Shows snow effect" ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Qml Gui SerialPort Concurrent REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

qt_add_executable(${PROJECT_NAME}
    main.cpp
    presentation/bridge/AppEngine.cpp
    presentation/bridge/AppEngine.hpp
)

if(LET_IT_SNOW)
    set(RESOURCE_FILES presentation/resources/images/snowflake.png)
    set(MAIN_QML_FILE presentation/qml/Main_Snow.qml)
else()
    set(MAIN_QML_FILE presentation/qml/Main.qml)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
add_definitions(-lwiringPi -lpthread)

add_subdirectory(gateway/Lora)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI AppQml
    VERSION 1.0

    QML_FILES
        ${MAIN_QML_FILE}
        presentation/qml/Pages/ConnectionPage.qml
        presentation/qml/Pages/ChatPage.qml

    RESOURCES
        presentation/resources/icons/chat.svg
        presentation/resources/icons/setting.svg
        ${RESOURCE_FILES}
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
        LoRaCore
)

include(GNUInstallDirs)
install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
