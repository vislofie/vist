#pragma once
#include <memory>
#include <asio/streambuf.hpp>
#include <asio/ip/tcp.hpp>

using asio::ip::tcp;
using asio::error_code;

class session: public std::enable_shared_from_this<session> {
public:
    session(tcp::socket&& socket);

    void start(std::function<void(std::string)>&& on_message, std::function<void()>&& on_error);
    void post(const std::string& message);

private:
    void async_read();
    void on_read(error_code error, std::size_t bytes_transferred);
    void async_write();
    void on_write(error_code error, std::size_t bytes_transferred);

    tcp::socket socket;
    asio::streambuf streambuf{};
    std::queue<std::string> outgoing{};
    std::function<void(std::string)> on_message{};
    std::function<void()> on_error;
};
