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
            std::array<char, PACKET_MAX_SIZE> buffer{};

            size_t len = socket.read_some(asio::buffer(buffer), ec);
            if (len == 0) {
                continue;
            }

            std::vector<std::unique_ptr<Message>> messages;
            size_t offset = 0;
            while (len - offset > 0) {
                uint8_t msg_sz;
                memcpy(&msg_sz, buffer.data() + offset, sizeof(uint8_t));

                offset += sizeof(uint8_t);

                auto rcv_msg = Message::create(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(buffer.data() + offset), msg_sz));
                messages.push_back(std::move(rcv_msg));

                offset += msg_sz;
            }

            for (auto& rcv_msg : messages) {
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
            }
        }

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}