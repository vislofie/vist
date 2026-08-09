#include "client_session.h"
#include <asio/read_until.hpp>
#include <asio/write.hpp>

#include "storage.h"

using namespace std::placeholders;
using asio::ip::tcp;

client_session::client_session(tcp::socket&& socket)
: m_socket(std::move(socket)) {

}

void client_session::start(std::function<void(std::string&)> &&on_message,
                    std::function<void()> &&on_error) {
    this->m_on_message = std::move(on_message);
    this->m_on_error = std::move(on_error);
    async_read();
}

void client_session::post(const std::string &message) {
    bool idle = m_outgoing_queue.empty();
    m_outgoing_queue.push(message);

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
    asio::async_read_until(
        m_socket,
        m_streambuf,
        "\n",
        [shared = shared_from_this()](error_code error, std::size_t bytes_transferred) {
            shared->on_read(error, bytes_transferred);
        });
}

void client_session::on_read(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        std::stringstream message;
        message << std::istream(&m_streambuf).rdbuf();
        m_streambuf.consume(bytes_transferred);
        std::string text = message.str();
        m_on_message(text);
        async_read();
    } else {
        m_socket.close(error);
        m_on_error();
    }
}

void client_session::async_write() {
    asio::async_write(
        m_socket,
        asio::buffer(m_outgoing_queue.front()),
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
