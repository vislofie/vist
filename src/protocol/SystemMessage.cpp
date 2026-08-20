#include "include/SystemMessage.h"

SystemMessage::SystemMessage()
 : m_message{}
 , m_system_message_type{} {
}

SystemMessage::SystemMessage(const std::string &msg, SystemMessageType msg_type) {
    m_message = msg;
    m_system_message_type = msg_type;
}

SystemMessage::~SystemMessage() {
    m_message.clear();
}

bool SystemMessage::deserialize(const std::span<const uint8_t> msg) {
    if (msg.size_bytes() < sizeof(get_message_type()))
        return false;

    uint32_t offset = sizeof(get_message_type());
    memcpy(&m_system_message_type, msg.data() + offset, sizeof(SystemMessageType));
    offset += sizeof(SystemMessageType);

    uint8_t msg_size;
    memcpy(&msg_size, msg.data() + offset, sizeof(uint8_t));
    offset += sizeof(uint8_t);

    m_message.resize(msg_size);
    memcpy(m_message.data(), msg.data() + offset, msg_size);

    return true;
}

std::vector<uint8_t> SystemMessage::serialize() const {
    std::vector<uint8_t> serialized_message;

    if (m_message.empty()) {
        return serialized_message;
    }

    serialized_message.push_back(static_cast<uint8_t>(get_message_type()));
    serialized_message.push_back(get_system_message_type());
    serialized_message.push_back(m_message.size());
    serialized_message.insert(serialized_message.end(), m_message.begin(), m_message.end());

    return serialized_message;
}

MessageType SystemMessage::get_message_type() const {
    return MessageType::System;
}

bool SystemMessage::is_empty() const {
    return m_message.empty();
}

std::string_view SystemMessage::get_message() {
    return m_message;
}

SystemMessageType SystemMessage::get_system_message_type() const {
    return m_system_message_type;
}
