#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "queue.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <sys/poll.h>
#include <utility>
#include <vector>

Hub::Hub(SocketAddress addr) {
    this->socket = new ServerSocket(addr);
    this->socket->bind();
    this->socket->listen();
}

void Hub::run() {
    while (true) {
        this->debugLogClients();
        this->accept();
        logInfo("Ready to accept next Client");
    }
}

void Hub::accept() {
    auto client_socket = this->socket->accept().unwrap();
    logInfo("New connection from " + client_socket.address()->fmt());

    auto client = Client(client_socket);

    this->addClient(client);

    logInfo("Added Client: " + client.fmt());
}

void Hub::addClient(Client client) {
    // Add Client to the pollfd vector for polling events
    pollfd poll_fd = {client.getFd(), POLLIN, 0};
    this->poll_fds.push_back(poll_fd);

    // Add Client to the general Client vec
    this->clients.push_back(std::make_pair(client, std::vector<QueuePtr>{}));
}

void Hub::removeClient(int fd) {
    (void)fd;
    puts("Unimplemented");
    exit(1);
}

void Hub::debugLogClients() {
    if (this->clients.empty()) {
        print("No Clients");
        return;
    }
    print("Current Clients:");
    for (auto& client : this->clients) {
        print(client.first.fmt());
    }
}

Hub::~Hub() {
    logWarn("Dropping Hub");
    delete this->socket;
}
