#include <iostream>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

using asio::ip::tcp;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << "<host> <port>" << std::endl;
    }

    try {
        asio::io_context io;
        tcp::resolver resolver(io);

        const auto endpoints = resolver.resolve(argv[1], argv[2]);

        tcp::socket socket(io);
        asio::connect(socket, endpoints);

        for (;;) {
            std::array<char, 128> buffer;
            std::error_code ec;

            size_t len = socket.read_some(asio::buffer(buffer), ec);

            if (ec == asio::error::eof) {
                break; // connection closed cleanly by peer
            }
            else if (ec) {
                throw std::system_error(ec);
            }

            std::cout.write(buffer.data(), len);
        }
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
