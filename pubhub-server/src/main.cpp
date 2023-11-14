#include "common.hpp"
#include "myresult.hpp"
#include "net/SocketAddress.hpp"
#include "core/client.hpp"
#include "core/hub.hpp"
#include "core/queue.hpp"
#include <cstdlib>
#include <iostream>
#include <stop_token>
#include "net/exceptions.hpp"

int main() {
    logInfo("Starting the PubHub Server...");
    auto hub_addr = SocketAddress("127.0.0.1", 8080);
    logInfo("Creating Hub...");
    auto hub = Hub(hub_addr);
    logInfo("Created a Hub instance");
    hub.run();
    logWarn("Shutdown...");
}

