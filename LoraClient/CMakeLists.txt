cmake_minimum_required(VERSION 3.16)

# options
option(LET_IT_SNOW "Shows snow effect" OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC OFF)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find libs
find_package(Qt6 COMPONENTS Core Qml Gui SerialPort Concurrent REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Core)

# lora lib
add_subdirectory(src/infrastructure/gateway/LoRaCore)

# add code files
qt_add_executable(${PROJECT_NAME}
    main.cpp

    # app
    src/application/AppEngine.hpp
    src/application/AppEngine.cpp

    # business-logic
    src/core/entities/MsgStructures.hpp

    src/core/usecases/ConnectionUseCase.cpp
    src/core/usecases/ConnectionUseCase.hpp

    src/core/usecases/ReceiveUseCase.cpp
    src/core/usecases/ReceiveUseCase.hpp

    src/core/usecases/SendUseCase.cpp
    src/core/usecases/SendUseCase.hpp

    # lora
    src/domain/interfaces/IConnectionWorker.hpp
    src/infrastructure/gateway/LoraWrapper.hpp

    # presentation
    src/presentation/models/UiStructures.hpp

    src/presentation/controller/QmlСontroller.hpp
    src/presentation/controller/QmlСontroller.cpp
)

# check snow option
if(LET_IT_SNOW)
    # snow image
    set(SNOW_RESOURCE_FILE       resources/images/snowflake.png)

    # elements
    set(SNOW_QML_FILE            qml/Elements/SnowBG.qml)

    # pages
    set(CONNECTION_PAGE_QML_FILE qml/Pages/ConnectionPage_Snow.qml)
    set(CHAT_PAGE_QML_FILE       qml/Pages/ChatPage_Snow.qml)

    # main page
    set(MAIN_QML_FILE qml/Main_Snow.qml)

    # add macro for c++
    add_compile_definitions(LET_IT_SNOW)
else()
    set(CONNECTION_PAGE_QML_FILE qml/Pages/ConnectionPage.qml)
    set(CHAT_PAGE_QML_FILE       qml/Pages/ChatPage.qml)

    # main page
    set(MAIN_QML_FILE qml/Main.qml)
endif()

# link libs
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)

qt_add_qml_module(
    ${PROJECT_NAME}
    URI AppQml
    VERSION 1.0

    QML_FILES
        qml/Main.qml
        ${SNOW_QML_FILE}
        ${CHAT_PAGE_QML_FILE}
        ${CONNECTION_PAGE_QML_FILE}
        ${MAIN_QML_FILE}

    RESOURCES
        resources/icons/chat.svg
        resources/icons/setting.svg
        ${SNOW_RESOURCE_FILE}
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
