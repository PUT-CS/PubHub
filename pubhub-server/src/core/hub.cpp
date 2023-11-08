#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "queue.hpp"
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

std::vector<int> fds;

Hub::Hub(SocketAddress addr) {
    this->socket = new ServerSocket(addr);
    this->socket->bind();
    this->socket->listen();
}

void Hub::run() {
    // Accept all clients
    while (true) {
        this->accept();
        logInfo("Ready to accept next Client");
    }
}

void Hub::accept() {
    // get the ClientSocket
    auto client_socket = this->socket->accept();
    logInfo("Got a new ClientSocket");
    logInfo(client_socket.get().fd);

    fds.push_back(client_socket.get().fd);
    // this->clients.push_back({std::move(Client(client_socket.get())),
    // std::vector<QueuePtr>{}});
    for (int fd : fds) {
        std::cout << "fd: " << fd << "\n";
    }
    // for (auto& pair : this->clients) {
    //     std::cout<< "CLIENT: " << pair.first.socket.address()->port << " " <<
    //     pair.first.socket.fd << std::endl;
    // }

    logInfo("Added client");
}

Hub::~Hub() {
    logWarn("Dropping Hub");
    delete this->socket;
}
