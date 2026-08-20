set(UNIT_NAME vist-server)

add_executable(${UNIT_NAME}
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/server.h
        ${CMAKE_CURRENT_SOURCE_DIR}/server.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/client_session.h
        ${CMAKE_CURRENT_SOURCE_DIR}/client_session.cpp
)

target_link_directories(${UNIT_NAME}
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src/protocol
    PRIVATE
        ${CMAKE_SOURCE_DIR}/src/storage
)
target_link_libraries(${UNIT_NAME}
    storage
    protocol
)

target_include_directories(${UNIT_NAME} PRIVATE
    ${LIB_PATH}/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/storage
    ${CMAKE_SOURCE_DIR}/src
)