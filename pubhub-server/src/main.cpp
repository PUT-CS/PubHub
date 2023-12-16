#include "common.hpp"
#include "server/Hub.hpp"
#include <string>

const uint16_t SERVER_PORT = 8080;
const auto SERVER_ADDR = "127.0.0.1";

auto main() -> int {
    const auto addr = SocketAddress(SERVER_ADDR, SERVER_PORT);
    auto server = Hub(addr);
    server.run();
    INFO("Shutdown");
}
