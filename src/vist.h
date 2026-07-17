#pragma once
#include "asio/io_context.hpp"
#include "asio/ip/tcp.hpp"

using asio::ip::tcp;
class vist {
public:
    vist(asio::io_context &io, int port);

private:
    void start_accept();
    void handle_accept(const asio::error_code& error, const std::shared_ptr<tcp::socket> &sock);
    void handle_write(const asio::error_code& error, size_t bytes_transferred);

    asio::io_context &m_io;

    tcp::acceptor m_acceptor;
    std::vector<tcp::socket> m_sockets;
};
