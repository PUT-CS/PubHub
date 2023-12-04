#include "libpubhub/common.hpp"
#include "libpubhub/server/exceptions.hpp"
#include "libpubhub/server/hub.hpp"
#include "libpubhub/server/message.hpp"
#include "libpubhub/server/types.hpp"
#include <algorithm>
#include <cstdint>
#include <exception>
#include <functional>
#include <string>

class PubHubServer {
  private:
    Hub hub;

  public:
    PubHubServer(const SocketAddress &addr) : hub(addr) {}
    ~PubHubServer() {}

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
        auto &client = hub.clientByFd(fd);
        auto msg = client.receiveMessage();

        auto kind = msg.at("kind");

        std::function<void()> handler{};
        std::string target_channel;
        try {
            target_channel = msg.at("channel");
        } catch(const std::exception& e){
            logError(e.what());
        }

        switch (Payload::stringMap.at(kind)) {
        case PayloadKind::Subscribe:
            handler = subscribeHandler(client, target_channel);
            break;
        case PayloadKind::Unsubscribe:
            handler = unsubscribeHandler(client, target_channel);
            break;
        case PayloadKind::CreateChannel:
            handler = channelCreationHandler(target_channel);
            break;
        case PayloadKind::DeleteChannel:
            handler = channelDeletionHandler(target_channel);
            break;
        case PayloadKind::Publish:
            // check if message has content
            handler = publishHandler(target_channel, msg.at("content"));
            break;
        default:
            logWarn("Reached default branch while handling input");
            for (auto i : hub.channels) {
                std::cout << i.first << " " + i.second.name << std::endl;
            }
            break;
        }

        auto msg_str = msg.dump();
        logInfo("Received from " + std::to_string(fd) + ": " + msg_str);

        try {
            logInfo("Calling handler");
            handler();
            logInfo("Exit handler");
        } catch(const InternalErrorException& e) {
            logError("\tINTERNAL ERROR: " + std::string(e.what()));
        } catch(const InvalidInputException& e) {
            logError("\tINVALID INPUT: " + std::string(e.what()));
        } catch(const std::exception& e) {
            logError("\tSOMETHING ELSE: " + std::string(e.what()));
        } catch (...) {
            logError("???");
            std::exception_ptr p = std::current_exception();
            std::clog << (p ? p.__cxa_exception_type()->name() : "null")
                      << std::endl;
        }
    }

    Hub::HandlerFn subscribeHandler(const Client& client, const ChannelName& target) {
        return [&]() {
            logWarn("\tSubscribe handler");
            hub.addSubscription(client.getFd(), target);
            hub.debugLogChannels();
        };
    }

    Hub::HandlerFn unsubscribeHandler(const Client& client, const ChannelName& target) {
        return [&]() {
            logWarn("\tUnsubscribe handler");
            hub.removeSubscription(client.getFd(), target);
            hub.debugLogChannels();
        };
    }

    Hub::HandlerFn channelCreationHandler(const ChannelName& target) {
        return [&]() {
            logWarn("\tAdding channel");
            hub.addChannel(target);
            hub.debugLogChannels();
        };
    }

    Hub::HandlerFn channelDeletionHandler(const ChannelName& target) {
        return [&](){
            logWarn("\tDeleting channel");
            hub.deleteChannel(target);
            hub.debugLogChannels();
        };
    }

    Hub::HandlerFn publishHandler(const ChannelName& target, const std::string& message) {
        return [&]() {
            logWarn("\tPublish handler");
        };
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

constexpr uint16_t SERVER_PORT = 8080;
constexpr auto SERVER_ADDR = "127.0.0.1";

int main() {
    const auto addr = SocketAddress(SERVER_ADDR, SERVER_PORT);
    auto server = PubHubServer(addr);
    logInfo("Server created");
    server.run();
    logInfo("Shutdown");
}
