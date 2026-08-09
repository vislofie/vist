#pragma once

#include <span>
#include "defines.h"

enum class MessageType : uint8_t {
    System = 0,
    ChatMessage,
};

struct alignas(64) Message {
    MessageType message_type{};
    uint16_t message_length{0};
    char m_data[PACKET_MAX_SIZE]{};

    explicit Message(const std::span<const uint8_t> msg) {
        deserialize(msg);
    }

    explicit Message(const std::string& message, const MessageType message_type) {
        if (message.size() > PACKET_MAX_SIZE) {
            throw std::length_error("Message too long");
        }

        this->message_type = message_type;
        this->message_length = message.length();

        memcpy(m_data, message.data(), message.size());
    }

    void deserialize(const std::span<const uint8_t> msg) {
        memcpy(&message_type, msg.data(), sizeof(MessageType));
        memcpy(&message_length, msg.data() + sizeof(MessageType), sizeof(uint16_t));

        if (message_length > PACKET_MAX_SIZE) {
            message_length = 0;
            message_type = MessageType::System;
            return;
        }

        memcpy(m_data, msg.data() + sizeof(MessageType) + sizeof(uint16_t), message_length);
    }

    [[nodiscard]] std::span<const uint8_t> serialize() const {
        return {
            reinterpret_cast<const uint8_t*>(this),
            sizeof(MessageType) + sizeof(uint16_t) + message_length
        };
    }

    [[nodiscard]] bool is_empty() const {
        return message_length == 0;
    }
};