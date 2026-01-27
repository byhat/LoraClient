cmake_minimum_required(VERSION 3.16)

project(LoraClient VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Qml Gui SerialPort Concurrent REQUIRED)
# GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

add_subdirectory(LoraClient)

enable_testing()
add_subdirectory(tests)

# documentation
file(GLOB DOC_FILES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.qmodel"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.md"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/**/*.drawio"
    # "${CMAKE_CURRENT_SOURCE_DIR}/doc/README*"
)

add_custom_target(doc_files SOURCES ${DOC_FILES})
