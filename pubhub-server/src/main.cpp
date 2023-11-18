#include <iostream>
#include "libpubhub/common.hpp"
#include "libpubhub/net/SocketAddress.hpp"
#include "libpubhub/server/hub.hpp"
#include "libpubhub/server/message.hpp"

int main() {
    nlohmann::json jason = {{"lorem", "ipsum"}};
    auto ppl = PublishPayload<Publish>(std::string("sebas"), 4, jason);
    std::cout << ppl.toString() << ppl.toString().size() << std::endl;
    auto bpl = BroadcastPayload(ppl);
    std::cout << bpl.toString() << bpl.toString().size() << std::endl;
    logInfo("Starting the PubHub Server...");
    auto hub_addr = SocketAddress("127.0.0.1", 8080);
    logInfo("Creating Hub...");
    auto hub = Hub(hub_addr);
    logInfo("Created a Hub instance");
    hub.run();
    logWarn("Shutdown...");
}

