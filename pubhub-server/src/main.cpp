#include "common.hpp"
#include "server/Hub.hpp"
#include <string>

// const uint16_t SERVER_PORT = 8080;
// const auto SERVER_ADDR = "127.0.0.1";

auto main(int argc, char* argv[]) -> int {
    if (argc < 3) {
        ERROR("Invalid number of arguments");
    }
    std::string ip = argv[1];
    int port = std::stoi(argv[2]);
    const auto addr = SocketAddress(ip, port);
    auto server = Hub(addr);
    server.run();
    INFO("Shutdown");
}
