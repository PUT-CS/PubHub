#include "common.hpp"
#include "json.hpp"
#include "common.hpp"
#include "server/event.hpp"
#include "server/exceptions.hpp"
#include "server/hub.hpp"
#include "server/message.hpp"
#include "server/types.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <string>

constexpr uint16_t SERVER_PORT = 8080;
constexpr auto SERVER_ADDR = "127.0.0.1";

int main() {
    const auto addr = SocketAddress(SERVER_ADDR, SERVER_PORT);
    auto server = Hub(addr);
    logInfo("Server created");
    server.run();
    logInfo("Shutdown");
}
