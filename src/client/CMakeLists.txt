set(UNIT_NAME vist-client)

add_executable(vist-client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_include_directories(vist-client PRIVATE
    ${LIB_PATH}/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/storage
)