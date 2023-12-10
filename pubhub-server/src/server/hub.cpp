#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>

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

    nlohmann::json request;
    try {
        request = client.receiveMessage();
    } catch (const NetworkException& e) {
        logError(e.what());
        auto response = Response::NetworkError(e.what());
        client.sendResponse(response);
    } catch (const nlohmann::json::parse_error& e) {
        logError(e.what());
        auto response = Response::InternalError();
        client.sendResponse(response);
    }

    std::string target_channel;
    try {
        target_channel = request.at("channel");
    } catch (const std::exception &e) {
        logError(e.what());
        return;
    }

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
        print(request.at("content"));
        handler = publishHandler(target_channel, request.at("content"));
        break;
    case RequestKind::Ask:
        handler = askHandler();
        break;
    default:
        logWarn("Reached default branch while handling input");
        break;
    }

    auto msg_str = request.dump();
    logInfo("Received from " + std::to_string(fd) + ": " + msg_str);

    Response res{};
    try {
        res = handler();
    } catch (const InternalErrorException &e) {
        res = Response::InternalError();
        logError("\tINTERNAL ERROR: " + std::string(e.what()));
    } catch (const InvalidInputException &e) {
        res = Response::InvalidRequest(e.what());
        logError("\tINVALID INPUT: " + std::string(e.what()));
    } catch (const std::exception &e) {
        res = Response::InternalError();
        logError("\tOTHER ERROR: " + std::string(e.what()));
    }
    client.sendResponse(res);
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
        logWarn(message);
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
    state_controller.removeClientByFd(fd);
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
