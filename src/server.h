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
    void post_from(std::shared_ptr<session> from, const std::string& message) const;
    void post_all(const std::string& message) const;

private:
    bool is_user_logged_in(std::string_view username) const;
    void process_client_message(std::shared_ptr<session> client, std::string& msg) const;

    io_context& m_ctxt;
    tcp::acceptor m_acceptor;
    std::optional<tcp::socket> m_socket;
    std::unordered_set<std::shared_ptr<session>> m_clients{};
};
