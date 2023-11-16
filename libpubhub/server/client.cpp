#include "client.hpp"
#include <cstdio>
#include <string>

Client::Client(ClientSocket socket) : socket(socket) {}

FileDescriptor Client::getFd() { return this->socket.fd; }

void Client::killConnection() {
    this->socket.shutdown();
    this->socket.close();
}

std::string Client::fmt() {
    return "FD: " + std::to_string(this->getFd()) +
        " ADDRESS: " + this->socket.address()->ip
        + ":" +
        std::to_string(this->socket.address()->port);
}

Client::~Client(){
    
}
