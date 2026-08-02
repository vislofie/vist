cmake_minimum_required(VERSION 4.2)
project(vist)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_PATH "$ENV{CPP_LIB}")
message("LIB PATH = ${LIB_PATH}")

set(STORAGE_TYPE "FILE")
message("STORAGE_TYPE = ${STORAGE_TYPE}")

add_compile_definitions(ASIO_STANDALONE)
add_compile_definitions("${STORAGE_TYPE}_STORAGE")

add_executable(vist
    src/main.cpp
    src/server.h
    src/server.cpp
    src/session.h
    src/session.cpp
    src/storage/storage.h
    src/storage/storage.cpp
    src/storage/impl/file/file_storage.h
    src/storage/impl/file/file_storage.cpp
)

target_include_directories(vist PRIVATE
    ${LIB_PATH}/asio/include
    ${CMAKE_SOURCE_DIR}/src
)