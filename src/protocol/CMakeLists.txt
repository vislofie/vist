set(UNIT_NAME protocol)

set(${UNIT_NAME}_SRC
    ${CMAKE_CURRENT_SOURCE_DIR}/Message.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/AuthMessage.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SystemMessage.cpp
)

set(${UNIT_NAME}_HDR
    ${CMAKE_CURRENT_SOURCE_DIR}/include/defines.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Message.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/SystemMessage.h
)

add_library(${UNIT_NAME} STATIC
    ${${UNIT_NAME}_SRC}
    ${${UNIT_NAME}_HDR}
)
