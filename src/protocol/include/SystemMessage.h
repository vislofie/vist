#pragma once
#include "Message.h"

enum SystemMessageType : uint8_t {
    Warning = 0,
    Error,
    Information
};

class SystemMessage : public Message {
public:
    SystemMessage();
    SystemMessage(const std::string& msg, SystemMessageType msg_type);

    ~SystemMessage() override;

    bool deserialize(const std::span<const uint8_t> msg) override;
    std::vector<uint8_t> serialize() const override;

    MessageType get_message_type() const override;

    bool is_empty() const override;

    std::string_view get_message();
    SystemMessageType get_system_message_type() const;

private:
    std::string m_message;
    SystemMessageType m_system_message_type;
};
