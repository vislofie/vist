cmake_minimum_required(VERSION 4.2)
project(vist)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_PATH "$ENV{CPP_LIB}")
message("LIB PATH = ${LIB_PATH}")

set(STORAGE_TYPE "FILE")
message("STORAGE_TYPE = ${STORAGE_TYPE}")

add_compile_definitions("${STORAGE_TYPE}_STORAGE")
add_compile_definitions(ASIO_STANDALONE)

add_subdirectory(src/protocol)
add_subdirectory(src/server)
add_subdirectory(src/client)

add_subdirectory(src/storage)