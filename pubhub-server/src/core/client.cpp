#include "client.hpp"
#include <cstdio>
#include <string>

Client::Client(ClientSocket socket) : socket(socket) {}

FileDescriptor Client::getFd() { return this->socket.fd; }

std::string Client::fmt() {
    return "FD: " + std::to_string(this->getFd()) +
        " ADDRESS: " + this->socket.address()->ip
        + ":" +
        std::to_string(this->socket.address()->port);
}

Client::~Client() {
    auto addr = this->socket.address();
    std::cerr << "Destroying Client: " << addr->ip << " " << addr->port
              << std::endl;
}
