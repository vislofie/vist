cmake_minimum_required(VERSION 4.2)
project(kv-srv)

set(CMAKE_CXX_STANDARD 20)

include_directories(kv-srv PRIVATE
    asio/include
)

add_executable(kv-srv
    main.cpp
)
