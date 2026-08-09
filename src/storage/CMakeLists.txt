set(UNIT_NAME storage)

set(${UNIT_NAME}_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/storage.cpp
)
set(${UNIT_NAME}_HDRS
    ${CMAKE_CURRENT_SOURCE_DIR}/storage.h
)

if (${STORAGE_TYPE} MATCHES "FILE")
    LIST(APPEND ${UNIT_NAME}_SRCS
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/file/file_storage.cpp
    )
    LIST(APPEND ${UNIT_NAME}_HDRS
        ${CMAKE_CURRENT_SOURCE_DIR}/impl/file/file_storage.h
    )
endif ()

add_library(${UNIT_NAME}
    ${${UNIT_NAME}_SRCS}
    ${${UNIT_NAME}_HDRS}
)

target_include_directories(${UNIT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)