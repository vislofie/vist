#pragma once
#include <memory>
#include <optional>
#include <unordered_set>
#include <asio/ip/tcp.hpp>
#include "asio.hpp"

#include "session.h"

using asio::ip::tcp;
using asio::io_context;
using asio::error_code;

class storage;

class server {
public:
    server(io_context& io_context, std::uint16_t port);

    void async_accept();
    void post(std::shared_ptr<session> from, const std::string& message) const;

private:
    io_context& io_context;
    tcp::acceptor acceptor;
    std::optional<tcp::socket> socket;
    std::unordered_set<std::shared_ptr<session>> clients{};

    std::shared_ptr<storage> m_storage{};
};
