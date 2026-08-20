#include "include/AuthMessage.h"

AuthMessage::AuthMessage() { }

AuthMessage::AuthMessage(const std::string& login, const std::string& password) {
    m_login = login;
    m_password = password;
}

AuthMessage::~AuthMessage() {
    m_login.clear();
    m_password.clear();
}

bool AuthMessage::deserialize(const std::span<const uint8_t> msg) {
    if (msg.size_bytes() == 0 || msg.size_bytes() < sizeof(uint8_t) * 3) {
        return false;
    }

    uint32_t offset = sizeof(MessageType);
    uint8_t part_size;
    memcpy(&part_size, msg.data() + offset, sizeof(part_size));
    offset += sizeof(part_size);

    m_login.resize(part_size);
    memcpy(m_login.data(), msg.data() + offset, part_size);
    offset += part_size;

    memcpy(&part_size, msg.data() + offset, sizeof(part_size));
    offset += sizeof(part_size);

    m_password.resize(part_size);
    memcpy(m_password.data(), msg.data() + offset, part_size);

    return true;
}

std::vector<uint8_t> AuthMessage::serialize() const {
    std::vector<uint8_t> result;

    result.push_back(static_cast<uint8_t>(get_message_type()));
    result.push_back(m_login.size());
    result.insert(result.end(), m_login.begin(), m_login.end());
    result.push_back(m_password.size());
    result.insert(result.end(), m_password.begin(), m_password.end());

    return result;
}

bool AuthMessage::is_empty() const {
    return m_login.empty() || m_password.empty();
}
