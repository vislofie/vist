#include "server.h"

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
        [this, weak = client->weak_from_this()](Message& msg) {
            if (const auto shared = weak.lock()) {
                process_client_message(shared, msg);
            }
        },
        [this, weak = client->weak_from_this()] {
            if (const auto shared = weak.lock();
                shared && m_clients.erase(shared)) {
                if (!shared->is_authorized())
                    return;

                post_all(Message(std::format("{} quit\n\r", shared->get_username()), MessageType::System));
            }
        });

        client->post(Message("You need to authorize. Your login: ", MessageType::System));
        async_accept();
    });
}

void server::post_from(const std::shared_ptr<client_session> &from, const Message &message) const {
    for (auto& client : m_clients) {
        if (client == from)
            continue;

        client->post(message);
    }
}

void server::post_all(const Message& message) const {
    for (auto& client : m_clients) {
        client->post(message);
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

void server::process_client_message(std::shared_ptr<client_session> client, const Message& msg) const {

}