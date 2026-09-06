#include "client_session.h"

#include <mutex>
#include <asio/read.hpp>
#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include "storage.h"
#include "protocol/include/defines.h"

using namespace std::placeholders;
using asio::ip::tcp;

client_session::client_session(tcp::socket&& socket)
: m_socket(std::move(socket)) {

}

void client_session::start(std::function<void(std::unique_ptr<Message>&)> &&on_message,
                    std::function<void()> &&on_error) {
    this->m_on_message = std::move(on_message);
    this->m_on_error = std::move(on_error);
    async_read();
}

void client_session::post(std::unique_ptr<Message>&& message) {
    std::lock_guard lock(m_outgoing_queue_mutex);

    bool idle = m_outgoing_queue.empty();
    m_outgoing_queue.push(std::move(message));

    if (idle) {
        async_write();
    }
}

bool client_session::is_authorized() const {
    return m_is_authorized;
}

void client_session::set_is_authorized(const bool is_authorized) {
    m_is_authorized = is_authorized;
}

std::string_view client_session::get_username() {
    return m_username;
}

void client_session::set_username(const std::string &username) {
    m_username = username;
}

std::string client_session::get_password() const {
    return storage::instance()->sync_read(m_username + "_pass");
}

void client_session::set_password(std::string_view password) const {
    storage::instance()->sync_write(m_username + "_pass", password);
}

tcp::socket & client_session::get_socket() {
    return m_socket;
}

void client_session::async_read() {
    asio::async_read(
        m_socket,
        asio::buffer(&m_msg_size, sizeof(m_msg_size)),
        [shared = shared_from_this()](error_code error, std::size_t bytes_transferred) {
            shared->on_read_header(error, bytes_transferred);
    });
}

void client_session::on_read_header(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        if (m_msg_size == 0) {
            async_read();
            return;
        }

        asio::async_read(
            m_socket,
            asio::buffer(m_msg_payload, m_msg_size),
            [shared = shared_from_this()](error_code error, std::size_t bytes_transferred) {
                shared->on_read_body(error, bytes_transferred);
            }
        );
    } else {
        m_socket.close(error);
        m_on_error();
    }
}

void client_session::on_read_body(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        auto msg = Message::create(std::span(m_msg_payload, m_msg_size));
        m_on_message(msg);

        async_read();
    }
    else {
        m_socket.close(error);
        m_on_error();
    }
}

void client_session::async_write() {
    auto serialized_msg = m_outgoing_queue.front()->serialize();
    auto buffer = asio::buffer(serialized_msg);

    asio::async_write(
        m_socket,
        buffer,
        [shared = shared_from_this()](error_code error, std::size_t bytes_transferred) {
            shared->on_write(error, bytes_transferred);
        }
    );
}

void client_session::on_write(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        m_outgoing_queue.pop();

        if (!m_outgoing_queue.empty()) {
            async_write();
        }
    } else {
        m_socket.close(error);
        m_on_error();
    }
}
