#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "event.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <exception>
#include <mutex>
#include <string>
#include <thread>
#include <unistd.h>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    state_controller.registerPollFdFor(this->socket->fd);
}

void Hub::run() {
    logInfo("Starting the PubHub Server...");
    Event event;
    while (true) {
        logInfo("Waiting for the next event");
        while ((event = state_controller.nextEvent()).kind == EventKind::Nil) {}
        this->handleEvent(event);
        logInfo("\n\nNEXT\n");
    }
}

void Hub::handleEvent(Event event) {
    // purely a Server event
    if (event.kind == EventKind::ConnectionRequest) {
        this->handleNewConnection();
        return;
    }
    
    // Clear client events, preventing next `poll`s from reporting anything here
    logInfo("Clearing events...");
    state_controller.clearEventsByFd(event.fd);

    // Disable polling for this client
    logInfo("Disabling polling...");
    state_controller.setPollingByFd(event.fd, false);

    switch (event.kind) {
    case EventKind::Input:
        std::thread([&]() {
            this->handleInput(event.fd);
            logInfo("Enabling polling");
            state_controller.setPollingByFd(event.fd, true);
        }).detach();
        break;
    case EventKind::Disconnect:
        this->handleDisconnect(event.fd);
        break;
    default:
        logError("Unreachable branch reached");
        std::exit(1);
        break;
    }
}

void Hub::handleInput(FileDescriptor fd) {
    Client &client = state_controller.clientByFd(fd);
    Response res;
    
    try {
        nlohmann::json request = client.receiveMessage();

        auto msg_str = request.dump();
        logInfo("Received from " + std::to_string(fd) + ": " + msg_str);

        auto target_channel = request.at("channel");

        auto kind_str = request.at("kind");
        auto kind = Request::fromString(kind_str);
        HandlerFn handler{};

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
            handler = publishHandler(target_channel, request.at("content"));
            break;
        case RequestKind::Ask:
            handler = askHandler();
            break;
        default:
            logWarn("Reached default branch while handling input");
            break;
        }
        res = handler();
    } catch (const InvalidInputException& e) {
        logError("Invalid Input: " + std::string(e.what()));
        res = Response::InvalidRequest(e.what());
    } catch (...) {
        logError("Internal Error");
        res = Response::InternalError();
    }
    
    try {
        client.sendResponse(res);
    } catch (...) {
        logError("Failed to send response");
        state_controller.removeClientByFd(client);
    }
}

Hub::HandlerFn Hub::subscribeHandler(const Client &client,
                                     const ChannelName &target) {
    return [=]() {
        logWarn("\tSubscribe handler");
        state_controller.addSubscription(client.getFd(), target);
        state_controller.debugLogChannels();
        return Response::Ok();
    };
}

Hub::HandlerFn Hub::unsubscribeHandler(const Client &client,
                                       const ChannelName &target) {
    return [=]() {
        logWarn("\tUnsubscribe handler");
        state_controller.removeSubscription(client.getFd(), target);
        state_controller.debugLogChannels();
        return Response::Ok();
    };
}

Hub::HandlerFn Hub::createChannelHandler(const ChannelName &target) {
    return [=]() {
        logWarn("\tAdding channel");
        state_controller.addChannel(target);
        state_controller.debugLogChannels();
        return Response::Ok();
    };
}

Hub::HandlerFn Hub::deleteChannelHandler(const ChannelName &target) {
    return [=]() {
        logWarn("\tDeleting channel");
        state_controller.deleteChannel(target);
        state_controller.debugLogChannels();
        return Response::Ok();
    };
}

Hub::HandlerFn Hub::publishHandler(const ChannelName &target,
                                   const nlohmann::json &message) {
    return [=]() {
        logWarn("\tPublish handler");
        auto channel = state_controller.channelById(state_controller.channelIdByName(target));
        for (auto &sub_id : channel.subscribers) {
            logInfo("\tHandling sub of id: " + std::to_string(sub_id));
            auto &subscriber = state_controller.clientByFd(sub_id);
            auto msg =
                nlohmann::json{{"channel", target}, {"content", message}};
            subscriber.publishMessage(msg);
        }
        return Response::Ok();
    };
}

Hub::HandlerFn Hub::askHandler() {
    return [=]() {
        logWarn("Unfinished handler...");
        for (auto &i : state_controller.getChannels()) {
            std::cout << i.first << " " + i.second.name << std::endl;
        }
        return Response::OkWithContent("ASKCONTENT");
    };
}

void Hub::handleDisconnect(FileDescriptor fd) noexcept {
    auto& client = state_controller.clientByFd(fd);
    state_controller.removeClientByFd(client);
    logWarn("Client disconnected: " + std::to_string(fd));
}

void Hub::handleNewConnection() noexcept {
    Client client;
    try {
        client = this->accept();
    }
    catch (const NetworkException& e) {
        client.killConnection();
        logError(e.what());
        return;
    }
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
