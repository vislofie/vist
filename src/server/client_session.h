#pragma once
#include <memory>
#include <mutex>
#include <queue>
#include <asio/streambuf.hpp>
#include <asio/ip/tcp.hpp>

#include "protocol/include/defines.h"
#include "protocol/include/Message.h"

using asio::ip::tcp;
using asio::error_code;

class client_session: public std::enable_shared_from_this<client_session> {
public:
    client_session(tcp::socket&& socket);

    void start(std::function<void(std::unique_ptr<Message>&)>&& on_message,
               std::function<void()>&& on_error);
    void post(std::unique_ptr<Message>&& message);

    bool is_authorized() const;
    void set_is_authorized(bool is_authorized);

    std::string_view get_username();
    void set_username(const std::string &username);

    std::string get_password() const;
    void set_password(std::string_view password) const;

    tcp::socket& get_socket();

private:
    void async_read();
    void on_read_header(error_code error, std::size_t bytes_transferred);
    void on_read_body(error_code error, std::size_t bytes_transferred);
    void async_write();
    void on_write(error_code error, std::size_t bytes_transferred);

    tcp::socket m_socket;
    MSG_HEADER_SIZE m_msg_size{};
    uint8_t m_msg_payload[255];
    std::queue<std::unique_ptr<Message>> m_outgoing_queue{};
    std::mutex m_outgoing_queue_mutex{};
    std::function<void(std::unique_ptr<Message>&)> m_on_message{};
    std::function<void()> m_on_error;

    std::string m_username{};
    bool m_is_authorized{false};
};
