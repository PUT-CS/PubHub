#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    state_controller.registerPollFdFor(this->socket->fd);
}

void Hub::run() {
    logInfo("Starting the PubHub Server...");
    while (true) {
        auto event = state_controller.nextEvent();
        this->handleEvent(event);
    }
}

void Hub::handleEvent(Event event) {
    switch (event.kind) {
    case EventKind::Input:
        this->handleInput(event.fd);
        break;
    case EventKind::Disconnect:
        this->handleDisconnect(event.fd);
        break;
    case EventKind::ConnectionRequest:
        this->handleNewConnection();
        break;
    case EventKind::Nil:
        // Unreachable if nextEvent was called with a negative value
        break;
    }
}

void Hub::handleInput(FileDescriptor fd) {
    // Look for a client with an event
    // Client sent data and it's ready to read
    // TODO: Catch
    auto &client = state_controller.clientByFd(fd);
    auto msg = client.receiveMessage();

    std::string target_channel;
    try {
        target_channel = msg.at("channel");
    } catch (const std::exception &e) {
        logError(e.what());
        return;
    }

    auto kind_str = msg.at("kind");
    auto kind = Request::fromString(kind_str);
    std::function<void()> handler{};

    using Request::RequestKind;
    switch (kind) {
    case RequestKind::Subscribe:
        handler = subscribeHandler(client, target_channel);
        break;
    case RequestKind::Unsubscribe:
        handler = unsubscribeHandler(client, target_channel);
        break;
    case RequestKind::CreateChannel:
        handler = createChannelHandler(target_channel);
        break;
    case RequestKind::DeleteChannel:
        handler = deleteChannelHandler(target_channel);
        break;
    case RequestKind::Publish:
        // check if message has content, handle error
        handler = publishHandler(target_channel, msg.at("content"));
        break;
    case RequestKind::Ask:
        handler = askHandler();
        break;
    default:
        logWarn("Reached default branch while handling input");
        break;
    }

    auto msg_str = msg.dump();
    logInfo("Received from " + std::to_string(fd) + ": " + msg_str);

    try {
        logInfo("Calling handler");
        handler();
        logInfo("Exit handler");
    } catch (const InternalErrorException &e) {
        logError("\tINTERNAL ERROR: " + std::string(e.what()));
    } catch (const InvalidInputException &e) {
        logError("\tINVALID INPUT: " + std::string(e.what()));
    } catch (const std::exception &e) {
        logError("\tOTHER ERROR: " + std::string(e.what()));
    }
}

Hub::HandlerFn Hub::subscribeHandler(const Client &client,
                                     const ChannelName &target) {
    return [&]() {
        logWarn("\tSubscribe handler");
        state_controller.addSubscription(client.getFd(), target);
        state_controller.debugLogChannels();
    };
}

Hub::HandlerFn Hub::unsubscribeHandler(const Client &client,
                                       const ChannelName &target) {
    return [&]() {
        logWarn("\tUnsubscribe handler");
        state_controller.removeSubscription(client.getFd(), target);
        state_controller.debugLogChannels();
    };
}

Hub::HandlerFn Hub::createChannelHandler(const ChannelName &target) {
    return [&]() {
        logWarn("\tAdding channel");
        state_controller.addChannel(target);
        state_controller.debugLogChannels();
    };
}

Hub::HandlerFn Hub::deleteChannelHandler(const ChannelName &target) {
    return [&]() {
        logWarn("\tDeleting channel");
        state_controller.deleteChannel(target);
        state_controller.debugLogChannels();
    };
}

Hub::HandlerFn Hub::publishHandler(const ChannelName &target,
                                   const std::string &message) {
    return [&]() {
        logWarn("\tPublish handler");
        auto channel = state_controller.channelById(state_controller.channelIdByName(target));
        for (auto &sub_id : channel.subscribers) {
            logInfo("\tHandling sub of id: " + std::to_string(sub_id));
            auto &subscriber = state_controller.clientByFd(sub_id);
            auto msg =
                nlohmann::json{{"channel", target}, {"content", message}};
            subscriber.publishMessage(msg);
        }
    };
}

Hub::HandlerFn Hub::askHandler() {
    return [&]() {
        logWarn("Unfinished handler...");
        for (auto &i : state_controller.getChannels()) {
            std::cout << i.first << " " + i.second.name << std::endl;
        }
    };
}

void Hub::handleDisconnect(FileDescriptor fd) {
    state_controller.removeClientByFd(fd);
    logWarn("Client disconnected: " + std::to_string(fd));
}

void Hub::handleNewConnection() {
    auto client = this->accept();
    state_controller.addClient(client);
    logInfo("Added Client: " + client.fmt());
}

/**
   Throws:
   - **NetworkException** if either accept() or connect() fail
**/
Client Hub::accept() {
    auto client_socket = this->socket->accept();
    auto client = Client(client_socket);

    auto hub_port = this->socket->address().getPort();
    client.initializeBroadcast(hub_port + 1);

    return client;
}

Hub::~Hub() { logWarn("Dropping Hub"); }
