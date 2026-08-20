set(UNIT_NAME vist-client)

add_executable(vist-client
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/session.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/session.h
)

target_link_directories(${UNIT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src/protocol
)

target_link_libraries(${UNIT_NAME}
    protocol
)

target_include_directories(vist-client PRIVATE
    ${LIB_PATH}/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)