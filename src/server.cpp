#include "server.h"
#include "storage/storage.h"

using namespace std::placeholders;
server::server(asio::io_context &io_context, std::uint16_t port)  :
        io_context(io_context),
        acceptor(io_context, tcp::endpoint(tcp::v4(), port)) {
    m_storage = storage::create();
    async_accept();
}

void server::async_accept() {
    socket.emplace(io_context);

    acceptor.async_accept(*socket, [&](error_code error) {
        auto client = std::make_shared<session>(std::move(*socket));
        post("We have a newcomer\n\r");

        clients.insert(client);

        client->start(
            [this, client](std::string msg) {
                post(client, msg);
            },
            [&, weak = std::weak_ptr(client)] {
                if (auto shared = weak.lock();
                    shared && clients.erase(shared)) {
                    post(client, "We are one less\n\r");
                }
            });

        async_accept();
    });
}

void server::post(const std::string &message) const {
    for (auto& client : clients) {
        client->post(message);
    }
}
