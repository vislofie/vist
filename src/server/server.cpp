#include "server.h"

#include "protocol/include/SystemMessage.h"

using namespace std::placeholders;
server::server(io_context &io_context, const std::uint16_t port)  :
        m_ctxt(io_context),
        m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    async_accept();
}

void server::async_accept() {
    m_socket.emplace(m_ctxt);

    m_acceptor.async_accept(*m_socket, [this](error_code error) {
        const auto client = std::make_shared<client_session>(std::move(*m_socket));
        m_clients.insert(client);

        client->start(
        [this, weak = client->weak_from_this()](std::unique_ptr<Message>& message) {
            if (const auto shared = weak.lock()) {
                process_client_message(shared, message);
            }
        },
        [this, weak = client->weak_from_this()] {
            if (const auto shared = weak.lock();
                shared && m_clients.erase(shared)) {
                if (!shared->is_authorized())
                    return;

                post_all(std::make_unique<SystemMessage>(std::format("{} quit\n\r", shared->get_username()), SystemMessageType::Information));
            }
        });


        client->post(std::make_unique<SystemMessage>(INFO_NEED_AUTH, SystemMessageType::Information));
        async_accept();
    });
}

void server::post_from(const std::shared_ptr<client_session> &from, std::unique_ptr<Message> message) const {
    for (auto& client : m_clients) {
        if (client == from)
            continue;

        client->post(std::move(message));
    }
}

void server::post_all(std::unique_ptr<Message> message) const {
    for (auto& client : m_clients) {
        client->post(std::move(message));
    }
}

bool server::is_user_logged_in(const std::string_view username) const {
    for (auto& client : m_clients) {
        if (client->get_username() == username && client->is_authorized()) {
            return true;
        }
    }

    return false;
}

void server::process_client_message(std::shared_ptr<client_session> client, const std::unique_ptr<Message> &message) const {
    if (message->get_message_type() == MessageType::System) {

    }
    else if (message->get_message_type() == MessageType::ChatMessage) {

    }
    else if (message->get_message_type() == MessageType::Authorization) {

    }
    else {
        assert(false);
    }
}