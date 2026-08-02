#include <iostream>
#include "asio.hpp"
#include "server/server.h"

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        }

        io_context io;
        server serv(io, std::stoi(argv[1]));
        io.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}
