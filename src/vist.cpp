#include "vist.h"

#include <iostream>

#include "asio/write.hpp"

using asio::ip::tcp;

vist::vist(asio::io_context &io, int port) :
m_io(io),
m_acceptor(io, tcp::endpoint(tcp::v4(), port)) {
    start_accept();
}

void vist::start_accept() {
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(m_io);
    m_acceptor.async_accept(*socket,
    [this, socket](const asio::error_code& error) { this->handle_accept(error, socket); });
}

void vist::handle_accept(const asio::error_code& error, const std::shared_ptr<tcp::socket> &sock) {
    if (!error) {
        std::string msg = "Hello!";
        std::string response = std::format("HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: {}\r\n"
                             "Connection: close\r\n"
                             "\r\n"
                             "{}", msg.size(), msg);

        asio::async_write(*sock, asio::buffer(response),
        [this](const asio::error_code& error, size_t bytes_transferred) {
            handle_write(error, bytes_transferred);
        });
    }

    start_accept();
}

void vist::handle_write(const asio::error_code &error, size_t bytes_transferred) {
    std::cout << "Transferred " << bytes_transferred << " bytes";
}
