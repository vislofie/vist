#include "session.h"
#include <asio/read_until.hpp>
#include <asio/write.hpp>

using namespace std::placeholders;
using asio::ip::tcp;

session::session(tcp::socket&& socket)
: socket(std::move(socket)) {

}

void session::start(std::function<void(std::string)> &&on_message, std::function<void()> &&on_error) {
    this->on_message = std::move(on_message);
    this->on_error = std::move(on_error);
    async_read();
}

void session::post(const std::string &message) {
    bool idle = outgoing.empty();
    outgoing.push(message);

    if (idle) {
        async_write();
    }
}

void session::async_read() {
    asio::async_read_until(
        socket,
        streambuf,
        "\n",
        std::bind(&session::on_read, shared_from_this(), _1, _2));
}

void session::on_read(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        std::stringstream message;
        message << socket.remote_endpoint(error) << ": "
                << std::istream(&streambuf).rdbuf();
        streambuf.consume(bytes_transferred);
        on_message(message.str());
        async_read();
    } else {
        socket.close(error);
        on_error();
    }
}

void session::async_write() {
    asio::async_write(
            socket,
            asio::buffer(outgoing.front()),
            std::bind(&session::on_write, shared_from_this(), _1, _2));
}

void session::on_write(error_code error, std::size_t bytes_transferred) {
    if (!error) {
        outgoing.pop();

        if (!outgoing.empty()) {
            async_write();
        }
    } else {
        socket.close(error);
        on_error();
    }
}
