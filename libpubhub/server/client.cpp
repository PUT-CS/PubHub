#include "client.hpp"
#include "message.hpp"
#include "types.hpp"
#include <cstdio>
#include <optional>
#include <string>
#include <sys/socket.h>

Client::Client(ClientSocket socket) : socket(socket) { }

FileDescriptor Client::getFd() { return this->socket.fd; }

void Client::subscribeTo(ChannelId id) noexcept {
    subscriptions.insert(id);
}

void Client::unsubscribeFrom(ChannelId id) noexcept {
    subscriptions.erase(id);
}

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
   throws:
   -NetworkException if socket.receive() fails
   -json parse exception if it fails
 **/
nlohmann::json Client::receiveMessage() {
    return nlohmann::json::parse(socket.receive());
}

void Client::sendMessage(const Payload &message) {
    this->socket.send(message.getContent().dump());
}


std::string Client::fmt() {
    return "FD: " + std::to_string(this->getFd()) +
           " ADDRESS: " + this->socket.address().ip + ":" +
           std::to_string(this->socket.address().port);
}

Client::Client(){}
Client::~Client() {}
