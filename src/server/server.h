#pragma once
#include <memory>
#include <optional>
#include <unordered_set>
#include <asio/ip/tcp.hpp>
#include "asio.hpp"

#include "client_session.h"
#include "../protocol/include/Message.h"

using asio::ip::tcp;
using asio::io_context;
using asio::error_code;

class storage;

class server {
public:
    server(io_context& io_context, std::uint16_t port);

    void async_accept();
    void post_from(const std::shared_ptr<client_session> &from, std::unique_ptr<Message> message) const;
    void post_all(std::unique_ptr<Message> message) const;

private:
    bool is_user_logged_in(std::string_view username) const;
    void process_client_message(std::shared_ptr<client_session> client, const std::unique_ptr<Message> &message) const;

    io_context& m_ctxt;
    tcp::acceptor m_acceptor;
    std::optional<tcp::socket> m_socket;
    std::unordered_set<std::shared_ptr<client_session>> m_clients{};
};
