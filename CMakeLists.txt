cmake_minimum_required(VERSION 4.2)
project(vist)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_PATH "$ENV{CPP_LIB}")
message("LIB PATH = ${LIB_PATH}")

add_compile_definitions(ASIO_STANDALONE)

add_executable(vist
    src/main.cpp
    src/server.h
    src/server.cpp
    src/session.h
    src/session.cpp
)

include_directories(vist PRIVATE
    ${LIB_PATH}/asio/include
)