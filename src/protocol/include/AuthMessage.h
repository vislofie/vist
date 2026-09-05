#pragma once
#include "Message.h"

class AuthMessage : public Message {
public:
    AuthMessage();
    AuthMessage(const std::string& login, const std::string& password);

    ~AuthMessage() override;

    bool deserialize(const std::span<const uint8_t> msg) override;

    MessageType get_message_type() const override { return MessageType::Authorization; }

    bool is_empty() const override;

    std::string_view get_login()    const noexcept { return m_login; }
    std::string_view get_password() const noexcept { return m_password; }

protected:
    std::vector<uint8_t> serialize_impl() const override;

private:
    std::string m_login;
    std::string m_password;
};
