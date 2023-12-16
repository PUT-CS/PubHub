#include "common.hpp"
#include "server/hub.hpp"
#include <cstddef>
#include <cstdlib>
#include <string>

const uint16_t SERVER_PORT = 8080;
const auto SERVER_ADDR = "127.0.0.1";

int main() {
    const auto addr = SocketAddress(SERVER_ADDR, SERVER_PORT);
    auto server = Hub(addr);
    INFO("Server created");
    server.run();
    INFO("Shutdown");
}
