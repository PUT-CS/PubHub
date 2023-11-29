#include "libpubhub/server/hub.hpp"

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
	// TODO: Catch
	auto& client = hub.clientByFd(fd);
	nlohmann::json buf;
	buf = client.receiveMessage();

	switch (hub.getPayloadKind_map()[buf["kind"]]) {
	case PayloadKind::Subscribe:
	    std::cout << hub.getPayloadKind_map()["Subscribe"] << std::endl;
	    break;
	case PayloadKind::Unsubscribe:
	    std::cout << hub.getPayloadKind_map()["Unsubscribe"] << std::endl;
	    break;
	case PayloadKind::CreateChannel:
	    std::cout << hub.getPayloadKind_map()["CreateChannel"] << std::endl;
	    break;
	case PayloadKind::DeleteChannel:
	    std::cout << hub.getPayloadKind_map()["DeleteChannel"] << std::endl;
	    break;
	case PayloadKind::Publish:
	    std::cout << hub.getPayloadKind_map()["Publish"] << std::endl;
	    break;
	default:
	    std::cout << hub.getPayloadKind_map()["Error"] << std::endl;
	    break;
	}
	
        auto msg = buf.dump();
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
