#pragma once

#include <span>

#include "defines.h"

enum class MessageType : uint8_t {
    System = 0,
    ChatMessage,
    Authorization
};

class Message {
public:
    virtual ~Message() = default;

    static std::unique_ptr<Message> create(const std::span<const uint8_t> msg);

    // TODO: prepend uint16_t length before EACH MESSAGE to read it properly on the server side

    virtual bool deserialize(const std::span<const uint8_t> msg) = 0;
    virtual std::vector<uint8_t> serialize() const = 0;
    virtual MessageType get_message_type() const = 0;

    virtual bool is_empty() const = 0;
};