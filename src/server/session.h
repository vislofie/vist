#pragma once
#include <memory>
#include <asio/streambuf.hpp>
#include <asio/ip/tcp.hpp>

using asio::ip::tcp;
using asio::error_code;

class session: public std::enable_shared_from_this<session> {
public:
    session(tcp::socket&& socket);

    void start(std::function<void(std::string&)>&& on_message,
               std::function<void()>&& on_error);
    void post(const std::string& message);

    bool is_authorized() const;
    void set_is_authorized(bool is_authorized);

    std::string_view get_username();
    void set_username(const std::string &username);

    std::string get_password() const;
    void set_password(std::string_view password) const;

    tcp::socket& get_socket();

private:
    void async_read();
    void on_read(error_code error, std::size_t bytes_transferred);
    void async_write();
    void on_write(error_code error, std::size_t bytes_transferred);

    tcp::socket m_socket;
    asio::streambuf m_streambuf{};
    std::queue<std::string> m_outgoing_queue{};
    std::function<void(std::string&)> m_on_message{};
    std::function<void()> m_on_error;

    std::string m_username{};
    bool m_is_authorized{false};
};
