#include <iostream>
#include <asio/connect.hpp>
#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include "protocol/include/defines.h"
#include "protocol/include/Message.h"
#include "protocol/include/AuthMessage.h"
#include "protocol/include/SystemMessage.h"

using asio::ip::tcp;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <host> <port>" << std::endl;
    }

    try {
        asio::io_context io;
        tcp::resolver resolver(io);
        std::error_code ec;

        const auto endpoints = resolver.resolve(argv[1], argv[2]);

        tcp::socket socket(io);
        asio::connect(socket, endpoints);

        for (;;) {
            std::array<char, PACKET_MAX_SIZE> buffer;

            size_t len = socket.read_some(asio::buffer(buffer), ec);
            if (len == 0) {
                continue;
            }

            auto rcv_msg = Message::create(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer.data()), len));
            if (rcv_msg == nullptr) {
                continue;
            }

            if (rcv_msg->get_message_type() == MessageType::System) {
                auto sys_msg = dynamic_cast<SystemMessage*>(rcv_msg.get());
                if (sys_msg->get_message() == INFO_NEED_AUTH) {
                    std::string username;
                    std::string password;

                    std::cout << "Type your username: ";
                    std::cin >> username;
                    std:: cout << "Type your password: ";
                    std::cin >> password;

                    if (username.find(' ') != std::string::npos || username.find('|') != std::string::npos) {
                        throw std::invalid_argument("username must not contain space or a vertical bar!");
                    }

                    AuthMessage msg(username, password);
                    auto serialized_msg = msg.serialize();

                    socket.write_some(asio::buffer(serialized_msg.data(), serialized_msg.size()), ec);

                    if (ec) {
                        std::cerr << ec.message() << std::endl;
                        return -1;
                    }
                }
            }
            else {
                assert(false);
            }

            auto rcv_msg_serialized = rcv_msg->serialize();
            std::cout.write(reinterpret_cast<char*>(rcv_msg_serialized.data()), static_cast<uint8_t>(rcv_msg_serialized.size()));
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
