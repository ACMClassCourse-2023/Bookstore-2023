cmake_minimum_required(VERSION 3.22)
project(Bookstore_2023)

set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(CMAKE_CXX_FLAGS "-std=c++17")

include_directories(.)
include_directories(src)

add_executable(code
        src/memoryio.h
        src/main.cpp)