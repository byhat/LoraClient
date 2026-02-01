cmake_minimum_required(VERSION 3.16)
find_package(Qt6 COMPONENTS Test REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/LoraClient/src)

set(CMAKE_AUTOMOC ON)

# Define a test executable that links against GoogleTest and the project sources
add_executable(loraclient_tests
    main_test.cpp
    ConnectionUseCase_test.cpp
    MockConnectionWorker.hpp
    ../LoraClient/src/core/usecases/ConnectionUseCase.cpp
    ../LoraClient/src/domain/interfaces/IConnectionWorker.hpp
)

# Link against GoogleTest and the main project target (if it provides a library)
# Assuming the main project target is an executable, we link only to gtest libraries.
# If a library target exists, replace LoraClient with that target name.

target_link_libraries(loraclient_tests PRIVATE gtest gmock gmock_main Qt6::Test)
add_test(NAME loraclient_tests COMMAND loraclient_tests)
