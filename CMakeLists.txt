cmake_minimum_required(VERSION 4.2)
project(vist)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(ASIO_STANDALONE)

add_executable(vist
    src/main.cpp
    src/vist.cpp
    src/vist.h
)

include_directories(vist PRIVATE
    asio/include
)