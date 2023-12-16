#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "exceptions.hpp"
#include <string>
#include <thread>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    state_controller.registerPollFdFor(this->socket->fd);
}

void Hub::run() {
    INFO("Starting the PubHub Server...");
    Event event;
    while (true) {
        INFO("Waiting for the next event...");
        while ((event = state_controller.nextEvent()).kind == EventKind::Nil) {
        }
        this->handleEvent(event);
        // INFO("\n\nNEXT\n");
    }
}

void Hub::handleEvent(Event event) {
    // purely a Server event
    if (event.kind == EventKind::ConnectionRequest) {
        this->handleNewConnection();
        return;
    }

    // Clear client events, preventing next `poll`s from reporting anything here
    // INFO("Clearing events...");
    state_controller.clearEventsByFd(event.fd);

    // Disable polling for this client
    // INFO("Disabling polling...");
    state_controller.setPollingByFd(event.fd, false);

    switch (event.kind) {
    case EventKind::Input:
        // using `&` here causes a crash
        std::thread([=]() {
            this->handleInput(event.fd);
            INFO("Enabling polling");
            state_controller.setPollingByFd(event.fd, true);
        }).detach();
        break;
    case EventKind::Disconnect:
        this->handleDisconnect(event.fd);
        break;
    default:
        ERROR("Unreachable branch reached");
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
        INFO("Received from " + std::to_string(fd) + ": " + msg_str);

        auto kind_str = request.at("kind");
        auto kind = RequestKind::fromString(kind_str);

        using RequestKind::RequestKind;
        switch (kind) {
        case RequestKind::Subscribe:
            res = handleSubscribe(client, request.at("channel"));
            break;
        case RequestKind::Unsubscribe:
            res = handleUnsubscribe(client, request.at("channel"));
            break;
        case RequestKind::CreateChannel:
            res = handleCreateChannel(request.at("channel"));
            break;
        case RequestKind::DeleteChannel:
            res = handleDeleteChannel(request.at("channel"));
            break;
        case RequestKind::Publish:
            // check if message has content, handle error
            res = handlePublish(request.at("channel"), request.at("content"));
            break;
        case RequestKind::Ask:
            res = handleAsk();
            break;
        default:
            WARN("Reached default branch while handling input");
            break;
        }
    } catch (const InvalidInputException &e) {
        ERROR("Invalid Input: " + std::string(e.what()));
        res = Response::InvalidRequest(e.what());
    } catch (const std::exception &e) {
        ERROR("Internal Error: " + std::string(e.what()));
        res = Response::InternalError();
    }

    try {
        client.sendResponse(res);
    } catch (...) {
        ERROR("Failed to send response");
        state_controller.removeClientByFd(client);
    }
}

Response Hub::handleSubscribe(const Client &client, const ChannelName &target) {
    DEBUG("Subscribe handler");
    state_controller.addSubscription(client.getFd(), target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

Response Hub::handleUnsubscribe(const Client &client,
                                const ChannelName &target) {
    DEBUG("Unsubscribe handler");
    state_controller.removeSubscription(client.getFd(), target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

Response Hub::handleCreateChannel(const ChannelName &target) {
    DEBUG("Adding channel");
    state_controller.addChannel(target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

Response Hub::handleDeleteChannel(const ChannelName &target) {
    DEBUG("Deleting channel");
    state_controller.deleteChannel(target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

Response Hub::handlePublish(const ChannelName &target,
                            const nlohmann::json &message) {
    DEBUG("Publishing");
    auto channel =
        state_controller.channelById(state_controller.channelIdByName(target));
    for (auto &sub_id : channel.subscribers) {
        INFO("Handling sub of id: " + std::to_string(sub_id));
        auto &subscriber = state_controller.clientByFd(sub_id);
        auto msg = nlohmann::json{{"channel", target}, {"content", message}};
        subscriber.publishMessage(msg);
    }
    return Response::Ok();
}

Response Hub::handleAsk() {
    if (state_controller.getChannels().empty()) {
        return Response::OkWithContent("<No Channels>");
    }

    std::ostringstream oss;
    for (auto &i : state_controller.getChannels()) {
        oss << i.second.name << ';';
    }
    DEBUG(oss.str());
    return Response::OkWithContent(oss.str());
}

void Hub::handleDisconnect(FileDescriptor fd) noexcept {
    auto &client = state_controller.clientByFd(fd);
    state_controller.removeClientByFd(client);
    WARN("Client disconnected: " + std::to_string(fd));
}

void Hub::handleNewConnection() noexcept {
    Client client;
    try {
        client = this->accept();
    } catch (const NetworkException &e) {
        client.killConnection();
        ERROR(e.what());
        return;
    }
    state_controller.addClient(client);
    INFO("Added Client: " + client.fmt());
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

Hub::~Hub() { WARN("Dropping Hub"); }
