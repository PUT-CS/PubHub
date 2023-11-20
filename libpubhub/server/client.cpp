#include "client.hpp"
#include <cstdio>
#include <string>

Client::Client(ClientSocket socket) : socket(socket) {}

FileDescriptor Client::getFd() { return this->socket.fd; }

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

std::string Client::fmt() {
    return "FD: " + std::to_string(this->getFd()) +
           " ADDRESS: " + this->socket.address()->ip + ":" +
           std::to_string(this->socket.address()->port);
}

Client::~Client() {}
