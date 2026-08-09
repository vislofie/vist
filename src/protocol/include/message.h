#pragma once

#include "defines.h"

struct alignas(64) Message {
    char m_data[PACKET_MAX_SIZE];
};