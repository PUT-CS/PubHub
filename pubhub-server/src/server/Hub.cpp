#include "Hub.hpp"
#include "../net/exceptions.hpp"
#include "Event.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <cstdio>
#include <string>
#include <thread>
#include <vector>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    state_controller.registerPollFdFor(this->socket->getFd());
}

void Hub::run() {
    INFO("Starting the PubHub Server...");
    Event event{};
    while (true) {
        //INFO("Waiting for the next event...");
        event = state_controller.nextEvent();
        this->handleEvent(event);
        this->sweep();
    }
}

// check for dead clients
void Hub::sweep() {
    for (auto& client : this->state_controller.getClients()) {
        if (client.second.backlogSize() > 100) {
            WARN("Client " + std::to_string(client.second.getFd()) + "is dead. Removing.");
            this->state_controller.removeClientByFd(client.second);
        }
    }
}

void Hub::handleEvent(Event event) {
    // DEBUG("Handling event");
    //  purely a Server event
    if (event.kind == EventKind::ConnectionRequest) {
        DEBUG("Handle New Connection");
        this->handleNewConnection();
        return;
    }

    switch (event.kind) {
    case EventKind::Input:
        DEBUG("Handle Input");
        this->handleInput(event.fd);
        break;
    case EventKind::Disconnect:
        DEBUG("Handle Disconnect");
        this->handleDisconnect(event.fd);
        break;
    case EventKind::OutputPossible:
        //DEBUG("Output Possible");
        this->handleOutput(event.fd);
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
        this->state_controller.removeClientByFd(client);
    }

    try {
        this->state_controller.setPollOutByFd(client.getFd(), true);
        DEBUG("TURN ON POLLOUT");
        client.enqueueMessage(Response(res).toJson().dump());
    } catch (...) {
        ERROR("Failed to send response");
        state_controller.removeClientByFd(client);
    }
}

void Hub::handleOutput(FileDescriptor fd) {
    // send a message to the client that can receive it
    //DEBUG("Output");
    auto &client = this->state_controller.clientByFd(fd);
    // if (!client.hasBacklog()) {
    //     return;
    // }
    DEBUG("Flushing to " + std::to_string(fd));
    client.flushOne();
    if (!client.hasBacklog()) {
        DEBUG("TURN OFF POLLOUT");
        this->state_controller.setPollOutByFd(fd, false);
    }
}

auto Hub::handleSubscribe(const Client &client, const ChannelName &target)
    -> Response {
    DEBUG("Subscribe handler");
    state_controller.addSubscription(client.getFd(), target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

auto Hub::handleUnsubscribe(const Client &client, const ChannelName &target)
    -> Response {
    DEBUG("Unsubscribe handler");
    state_controller.removeSubscription(client.getFd(), target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

auto Hub::handleCreateChannel(const ChannelName &target) -> Response {
    DEBUG("Adding channel");
    state_controller.addChannel(target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

auto Hub::handleDeleteChannel(const ChannelName &target) -> Response {
    DEBUG("Deleting channel");
    state_controller.deleteChannel(target);
    state_controller.debugLogChannels();
    return Response::Ok();
}

auto Hub::handlePublish(const ChannelName &target,
                        const nlohmann::json &message) -> Response {
    DEBUG("Publishing");
    auto channel =
        state_controller.channelById(state_controller.channelIdByName(target));
    for (auto &sub_id : channel.subscribers) {
        INFO("Handling sub of id: " + std::to_string(sub_id));
        auto &subscriber = state_controller.clientByFd(sub_id);
        auto msg = nlohmann::json{{"channel", target}, {"content", message}};
        this->state_controller.setPollOutByFd(subscriber.getFd(), true);
        subscriber.enqueueMessage(msg.dump());
    }
    return Response::Ok();
}

auto Hub::handleAsk() -> Response {
    if (state_controller.getChannels().empty()) {
        return Response::OkWithContent("<No Channels>");
    }

    std::ostringstream oss;
    // Channel names containing whitespace are forbidden
    for (auto &i : state_controller.getChannels()) {
        oss << i.second.name << '\n';
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
        ERROR(e.what());
        return;
    }

    INFO("Adding Client: " + client.fmt());
    state_controller.addClient(std::move(client));
}

/**
   Throws:
   - **NetworkException** if either accept() or connect() fail
**/
auto Hub::accept() -> Client {
    auto client_socket = this->socket->accept();
    auto client = Client(client_socket);
    return client;
}

Hub::~Hub() { WARN("Dropping Hub"); }
