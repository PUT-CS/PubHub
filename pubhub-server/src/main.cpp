#include <iostream>
#include <string>
#include "libpubhub/common.hpp"
#include "libpubhub/net/SocketAddress.hpp"
#include "libpubhub/net/exceptions.hpp"
#include "libpubhub/server/hub.hpp"
#include "libpubhub/server/message.hpp"
#include <algorithm>
#include <string>
#include <vector>
#include "libpubhub/server/exceptions.hpp"

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
	nlohmann::json msg;
	msg = client.receiveMessage();

	auto kind = msg.at("kind");
	
	switch (hub.getPayloadKind_map()[kind]) {
	case PayloadKind::Subscribe:
	    logInfo(kind);
	    handleSubscribe(client, msg.at("target"));
	    break;
	case PayloadKind::Unsubscribe:
	    logInfo(kind);
	    handleUnsubscribe(client, msg.at("target"));
	    break;
	case PayloadKind::CreateChannel:
	    logInfo(kind);
	    hub.addChannel(msg["target"]);
	    break;
	case PayloadKind::DeleteChannel:
	    logInfo(msg);
	    hub.deleteChannel(msg["target"]);
	    break;
	case PayloadKind::Publish:
	    logInfo(msg);
	    break;
	default:
	    logInfo(msg);
	    for (auto i : hub.channels) {
		std::cout<< i.first << " " + i.second.name << std::endl;
	    }
	    break;
	}
	
        auto msg_str = msg.dump();
        if (msg_str.ends_with('\n')) {
            msg_str.pop_back();
        }
        
        logInfo("Received from " + std::to_string(fd) + ": " + msg_str);
    }

    void handleSubscribe(Client client, ChannelName target) {
	try {
	    hub.addSubscription(client.getFd(), target);
	    client.sendMessage(UtilityPayload<PayloadKind::Subscribe>("OK"));
	} catch(ChannelNotFoundException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::NoSuchChannel));
	} catch(ClientException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::InternalError));
	}
    }

    void handleUnsubscribe(Client client, ChannelName target) {
	try {
	    hub.removeSubscription(client.getFd(), target);
	    client.sendMessage(UtilityPayload<PayloadKind::Unsubscribe>("OK"));
	} catch(ChannelNotFoundException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::NoSuchChannel));
	} catch(ClientException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::InternalError));
	}
    }    
    
    void handleChannelCreation(Client client, ChannelName target) {
	try {
	    hub.addChannel(target);
	    client.sendMessage(UtilityPayload<PayloadKind::CreateChannel>("OK"));
	} catch(ChannelAlreadyCreated &e) {
	    client.sendMessage(ErrorPayload(target, HubError::ChannelAlreadyExists));
	} catch(ClientException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::InternalError));
	}
    }

    void handleChannelDeletion(Client client, ChannelName target) {
	try {
	    hub.deleteChannel(target);
	    client.sendMessage(UtilityPayload<PayloadKind::DeleteChannel>("OK"));
	} catch(ChannelNotFoundException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::NoSuchChannel));
	} catch(ClientException &e) {
	    client.sendMessage(ErrorPayload(target, HubError::InternalError));
	}
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

ChannelId Channel::channel_id_gen = 0;

int main() {
    const auto addr = SocketAddress("127.0.0.1", 8080);
    auto server = MyServer(addr);
    logInfo("Server created");
    server.run();
    logInfo("Shutdown");
}
