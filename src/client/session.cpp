#include "session.h"

session::session(tcp::socket &socket, std::function<void(Message &)> &&on_read_message)
    : m_socket(std::move(socket)),
      m_on_read_message(std::move(on_read_message)) {
    m_read_thread = std::thread([this]() {
        for (;;) {

        }
    });
}

session::~session() {
    m_socket.close();
}
