cmake_minimum_required(VERSION 3.16)

project(LoraClient VERSION 1.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 COMPONENTS Core Qml Gui SerialPort Concurrent REQUIRED)

add_subdirectory(src)
