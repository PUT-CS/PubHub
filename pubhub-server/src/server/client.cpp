#include "client.hpp"
#include "../common.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <unistd.h>

Client::Client(ClientSocket socket) : socket(socket) {}

FileDescriptor Client::getFd() const noexcept { return this->socket.fd; }

/**
   Create a socket that will send the data to that client.
   The client must receive from that socket to get published messages.

   Throws:
   - **NetworkException** if connect() fails
 **/
void Client::initializeBroadcast(uint16_t to_port) {

    // The client must listen on a port 1 higher than the server
    auto old_addr = this->socket.address();
    auto broadcast_addr = SocketAddress(old_addr.getIp(), to_port);

    //this->broadcast_socket = ClientSocket(broadcast_addr);
    this->broadcast_socket = ClientSocket(broadcast_addr);
    
    this->broadcast_socket.connect();
    logInfo("Connected broadcast to " + broadcast_addr.fmt());
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
nlohmann::json Client::receiveMessage() {
    auto s = socket.receive();
    return nlohmann::json::parse(s);
}

void Client::publishMessage(nlohmann::json message) {
    logWarn("Here0");
    logWarn("\tSending to " + this->broadcast_socket.address().getIp() + ":" +
            std::to_string(this->broadcast_socket.address().getPort()));
    logWarn("Here1");
    auto msg_str = message.dump();
    logWarn("Here10");
    this->broadcast_socket.send(msg_str);
    logWarn("Here2");
    logInfo("Published " + message.dump(2) + " to " + this->fmt());
    logWarn("Here3");
}

void Client::sendResponse(const Response& response) {
    auto res_str = response.toJson().dump();
    logInfo("Responding with " + res_str);
    this->socket.send(res_str);
}

std::string Client::fmt() const noexcept {
    return "FD: " + std::to_string(this->getFd()) +
           " ADDRESS: " + this->socket.address().getIp() + ":" +
           std::to_string(this->socket.address().getPort());
}

Client::Client() {}

Client::~Client() {}
