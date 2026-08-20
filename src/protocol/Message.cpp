#include "include/Message.h"
#include "include/AuthMessage.h"
#include "include/SystemMessage.h"

std::unique_ptr<Message> Message::create(const std::span<const uint8_t> msg) {

    MessageType msg_type;

    memcpy(&msg_type, msg.data(), sizeof(msg_type));
    if (msg_type == MessageType::System) {
        auto deserialized_msg = std::make_unique<SystemMessage>();
        if (!deserialized_msg->deserialize(msg))
            return nullptr;

        return std::move(deserialized_msg);
    }
    else if (msg_type == MessageType::ChatMessage) {
        return nullptr;
    }
    else if (msg_type == MessageType::Authorization) {
        auto deserialized_msg = std::make_unique<AuthMessage>();
        if (!deserialized_msg->deserialize(msg))
            return nullptr;

        return std::move(deserialized_msg);
    }
    else {
        throw std::runtime_error("Unknown MessageType");
    }
}
