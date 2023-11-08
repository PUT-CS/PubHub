#include "client.hpp"
#include <cstdio>

Client::Client(ClientSocket socket) : socket(socket) {}
Client::~Client() {
    auto addr = this->socket.address();
    std::cerr<<"Destroying Client: " << addr->ip<< " " << addr->port << std::endl;
}
