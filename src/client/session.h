#pragma once
#include <thread>
#include <asio/ip/tcp.hpp>
#include "protocol/include/Message.h"

using asio::ip::tcp;
class session {
public:
    session(tcp::socket& socket, std::function<void(Message&)>&& on_read_message);
    ~session();

private:
    std::thread m_read_thread;

    tcp::socket m_socket;
    std::function<void(Message&)> m_on_read_message;
};
