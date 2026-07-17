#include "vist.h"

#include <iostream>
#include "asio.hpp"

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        }

        asio::io_context io;
        vist srv(io, std::stoi(argv[1]));
        io.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
}