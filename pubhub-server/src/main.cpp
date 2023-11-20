#include <iostream>
#include <string>
#include "libpubhub/common.hpp"
#include "libpubhub/net/SocketAddress.hpp"
#include "libpubhub/net/exceptions.hpp"
#include "libpubhub/server/hub.hpp"
#include "libpubhub/server/message.hpp"
#include <algorithm>
#include <csignal>
#include <string>
#include <thread>
#include <vector>

class MyServer {
  private:
    Hub hub;

  public:
    MyServer(const SocketAddress &addr) : hub(addr) {}
    ~MyServer() {}

    void run() {
        logInfo("Starting the PubHub Server...");
        while (true) {
            auto event = hub.nextEvent(-1);
            this->handleEvent(event);
        }
    }

    void handleEvent(const Event &event) {
        switch (event.kind) {
        case Input:
            this->handleInput(event.fd);
            break;
        case Disconnect:
            this->handleDisconnect(event.fd);
            break;
        case ConnectionRequest:
            this->handleConnectionRequest();
            break;
        case Nil:
            // Unreachable if nextEvent was called with a negative value
            break;
        }
    }

    void handleInput(FileDescriptor fd) {
        // Look for a client with an event
        // Client sent data and it's ready to read

        char buf[16] = {};
        int n = recv(fd, buf, 16, 0);
        if (n == -1) {
            throw NetworkException("Recv");
        }
        
        auto msg = std::string(buf);
        if (msg.ends_with('\n')) {
            msg.pop_back();
        }
        
        logInfo("Received from " + std::to_string(fd) + ": " + msg);
    }

    void handleDisconnect(FileDescriptor fd) {
        hub.removeClientByFd(fd);
        logWarn("Client disconnected: " + std::to_string(fd));
    }

    void handleConnectionRequest() {
        auto client = hub.accept();
        hub.addClient(client);
        logInfo("Added Client: " + client.fmt());
    }
};

int main() {
    const auto addr = SocketAddress("127.0.0.1", 8080);
    auto server = MyServer(addr);
    logInfo("Server created");
    server.run();
    logInfo("Shutdown");
}
