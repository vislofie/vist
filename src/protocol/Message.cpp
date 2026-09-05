#include <cstring>

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

std::vector<uint8_t> Message::serialize() {
    auto serialized_msg = serialize_impl();
    serialized_msg.insert(serialized_msg.begin(), static_cast<uint8_t>(serialized_msg.size()));

    return serialized_msg;
}
