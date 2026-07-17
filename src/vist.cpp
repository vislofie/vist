#include "vist.h"
#include "asio/read.hpp"
#include "asio/write.hpp"

#include <iostream>

using asio::ip::tcp;
constexpr uint8_t read_buff_size = 8192;

vist::vist(asio::io_context &io, int port) :
m_io(io),
m_acceptor(io, tcp::endpoint(tcp::v4(), port)) {
    m_read_buff = new char[read_buff_size];
    start_accept();
}

vist::~vist() {
    delete[] m_read_buff;
}

void vist::start_accept() {
    std::shared_ptr<tcp::socket> socket = std::make_shared<tcp::socket>(m_io);
    m_acceptor.async_accept(*socket,
    [this, socket](const asio::error_code& error) { this->handle_accept(error, socket); });
}

void vist::handle_accept(const asio::error_code& error, const std::shared_ptr<tcp::socket> &sock) {
    if (!error) {
        std::string msg = "cool shit dawg";
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

        asio::async_read(*sock, asio::buffer(m_read_buff, read_buff_size),
        [this](const asio::error_code &error, size_t bytes_transferred) {
            handle_read(error, bytes_transferred);
        });
    }

    start_accept();
}

void vist::handle_write(const asio::error_code &error, size_t bytes_transferred) {
    std::cout << "Transferred " << bytes_transferred << " bytes";
}

void vist::handle_read(const asio::error_code &error, size_t bytes_transferred) {
    std::cout.write(m_read_buff, bytes_transferred);
}
