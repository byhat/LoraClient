cmake_minimum_required(VERSION 3.16)
find_package(Qt6 COMPONENTS Test Gui REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/LoraClient/src)
include_directories(${CMAKE_SOURCE_DIR}/LoraClient)

set(CMAKE_AUTOMOC ON)

# Fetch spdlog for tests
include(FetchContent)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# Define a test executable that links against GoogleTest and the project sources
add_executable(loraclient_tests
    main_test.cpp
    ConnectionUseCase_test.cpp
    ReceiveUseCase_test.cpp
    SendUseCase_test.cpp
    SaveImageUseCase_test.cpp
    MockConnectionWorker.hpp
    ../LoraClient/src/core/usecases/ConnectionUseCase.cpp
    ../LoraClient/src/core/usecases/ReceiveUseCase.cpp
    ../LoraClient/src/core/usecases/SendUseCase.cpp
    ../LoraClient/src/core/usecases/SaveImageUseCase.cpp
    ../LoraClient/src/domain/interfaces/IConnectionWorker.hpp
    ../LoraClient/src/core/entities/MsgStructures.hpp
    ../LoraClient/src/infrastructure/loggining/ILogger.hpp
    ../LoraClient/src/infrastructure/loggining/SpdlogLogger.cpp
    ../LoraClient/src/infrastructure/loggining/SpdlogLogger.hpp
)

# Link against GoogleTest and the main project target (if it provides a library)
# Assuming the main project target is an executable, we link only to gtest libraries.
# If a library target exists, replace LoraClient with that target name.

target_link_libraries(loraclient_tests PRIVATE gtest gmock gmock_main Qt6::Test Qt6::Gui spdlog::spdlog)
add_test(NAME loraclient_tests COMMAND loraclient_tests)
