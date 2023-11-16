#include "libpubhub/common.hpp"
#include "libpubhub/net/SocketAddress.hpp"
#include "libpubhub/server/hub.hpp"

int main() {
    logInfo("Starting the PubHub Server...");
    auto hub_addr = SocketAddress("127.0.0.1", 8080);
    logInfo("Creating Hub...");
    auto hub = Hub(hub_addr);
    logInfo("Created a Hub instance");
    hub.run();
    logWarn("Shutdown...");
}

