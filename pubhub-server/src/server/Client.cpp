#include "Client.hpp"
#include "../common.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <string>

using nlohmann::json;

Client::Client(ClientSocket socket) : socket(socket) {}

auto Client::getSubscriptions() -> const std::set<ChannelId> & {
    return std::ref(this->subscriptions);
}

auto Client::getFd() const noexcept -> FileDescriptor {
    return this->socket.fd;
}

void Client::subscribeTo(ChannelId id) noexcept { subscriptions.insert(id); }

void Client::unsubscribeFrom(ChannelId id) noexcept { subscriptions.erase(id); }

/**
   Forcibly close the connection, ignore errors
 **/
void Client::killConnection() noexcept {
    /*
      ignore the possible shutdown exception,
      client may have already disconnected on their own
      and we just need to clean up.
    */
    try {
        this->socket.kill();
    } catch (...) {
    }
}

/**
   Throws:
   - **NetworkException** if socket.receive() fails
   - **json::parse_exception** if it fails
 **/
auto Client::receiveMessage() -> json {
    auto s = socket.receive();
    return nlohmann::json::parse(s);
}

// void Client::publishMessage(const std::string& message) {
//     WARN("\tSending to " + this->socket.address().getIp() + ":" +
//          std::to_string(this->socket.address().getPort()));
//     this->socket.send(message);
//     // INFO("Published " + message.dump(2) + " to " + this->fmt());
// }

// void Client::sendResponse(const Response &response) {
//     auto res_str = response.toJson().dump();
//     INFO("Responding with " + res_str);
//     this->socket.send(res_str);
// }

void Client::enqueueMessage(const std::string& msg) {
    DEBUG("Enqueue: " + msg);
    this->backlog_queue.push_back(std::move(msg));
    DEBUG(std::to_string(this->backlog_queue.size()));
}

auto Client::backlogSize() -> size_t { return backlog_queue.size(); }

void Client::flushOne() {
    auto msg = this->backlog_queue.front();
    this->backlog_queue.pop_front();
    DEBUG("Flushing: " + msg);
    this->socket.send(msg);
    // check if disable POLLOUT
}

auto Client::hasBacklog() -> bool {
    return this->backlog_queue.size() > 0;
}

auto Client::fmt() const noexcept -> std::string {
    return "FD: " + std::to_string(this->getFd()) +
           " ADDRESS: " + this->socket.address().getIp() + ":" +
           std::to_string(this->socket.address().getPort());
}

