#include "server.h"
#include "storage/storage.h"

using namespace std::placeholders;
server::server(io_context &io_context, std::uint16_t port)  :
        m_ctxt(io_context),
        m_acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    async_accept();
}

void server::async_accept() {
    m_socket.emplace(m_ctxt);

    m_acceptor.async_accept(*m_socket, [this](error_code error) {
        auto client = std::make_shared<session>(std::move(*m_socket));
        m_clients.insert(client);

        client->start(
        [this, weak = client->weak_from_this()](std::string& msg) {
            if (auto shared = weak.lock()) {
                process_client_message(shared, msg);
            }
        },
        [this, weak = client->weak_from_this()] {
            if (auto shared = weak.lock();
                shared && m_clients.erase(shared)) {
                if (!shared->is_authorized())
                    return;

                post_all(std::format("{} quit\n\r", shared->get_username()));
            }
        });

        client->post("You need to authorize. Your login: ");
        async_accept();
    });
}

void server::post_from(std::shared_ptr<session> from, const std::string &message) const {
    for (auto& client : m_clients) {
        if (client == from)
            continue;

        client->post(message);
    }
}

void server::post_all(const std::string &message) const {
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

void server::process_client_message(std::shared_ptr<session> client, std::string& msg) const {
    std::erase_if(msg, [](const char c){ return c == '\n' || c == '\r' || c == '|'; });

    if (client->get_username().empty()) {
        if (msg.find(' ') != std::string::npos) {
            client->post("You can't use spaces in the username\n\r");
        }
        else if (msg.size() > 32) {
            client->post("Username can't be more than 32 symbols\n\r");
        }
        else if (is_user_logged_in(msg)) {
            client->post(std::format("{} already logged in\n\r", msg));
        }
        else {
            client->set_username(msg);
            client->post("Type your password: ");
        }
    }
    else if (!client->is_authorized()) {
        auto existing_password = client->get_password();

        if (msg.find(' ') != std::string::npos) {
            client->post("You can't use spaces in the username\n\r");
        }
        else if (existing_password.empty()) {
            client->set_password(msg);
            client->set_is_authorized(true);
        }
        else {
            if (existing_password != msg) {
                client->post("Wrong password.\n\r");

                client->get_socket().close();
            }
            else {
                client->set_is_authorized(true);
                client->post(std::format("Welcome {}\n\r", client->get_username()));
            }
        }
    }
    else {
        post_from(client, std::format("{}: {}\n\r", client->get_username(), msg));
    }
}