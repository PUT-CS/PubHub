#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "queue.hpp"
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>
#include <sys/poll.h>
#include <utility>
#include <vector>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = new ServerSocket(addr);
    this->socket->bind();
    this->socket->listen();

    pollfd server_pollfd = {this->socket->fd, POLLIN, 0};
    this->poll_fds.push_back(server_pollfd);
    logInfo("Registered server pollfd");
    this->debugLogPollFds();
}

void Hub::run() {
    while (true) {
        this->debugLogClients();
        this->handleNextEvent();
        //this->accept();
        //logInfo("Ready to accept next Client");
    }
}

/**
   Wait for the next event on either the server or one of the clients
 **/
void Hub::handleNextEvent() {
    int n_of_events = poll(this->poll_fds.data(), this->poll_fds.size(), -1);
    if (n_of_events == -1) {
        perror("Poll");
        return;
    }

    // Handle a server input event
    if (poll_fds[0].revents & POLLIN) {
        logInfo("Received server POLLIN event");
        return;
    }

    // Look for a client with an event
    for (int i = 1; i < poll_fds.size() - 1; i++) {
        auto fd = this->poll_fds[i];
        if (fd.revents & POLLIN) {
            logInfo("Received client POLLIN event");
        }
    }
    
    // Handle a client event
    // for (size_t i = 0; i < this->poll_fds.size(); ++i) {
    //     if (this->poll_fds[i].revents & POLLIN) {
    //         if (this->poll_fds[i].fd == serverSocket) {
    //             int clientSocket = acceptClientConnection(serverSocket);
    //             if (clientSocket != -1) {
    //                 this->poll_fds.push_back({clientSocket, POLLIN |
    //                 POLLRDHUP, 0}); std::cout << "New client connected on
    //                 socket "
    //                           << clientSocket << std::endl;
    //             }
    //         } else {
    //             handleClientData(this->poll_fds[i].fd, this->poll_fds);
    //         }
    //     }
    // }
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

Client* Hub::clientByFd(FileDescriptor fd) {
    
}

void Hub::debugLogClients() {
    if (this->clients.empty()) {
        print("No Clients");
        return;
    }
    print("Current Clients:");
    for (auto &client : this->clients) {
        print(client.first.fmt());
    }
}

void Hub::debugLogPollFds() {
    if (this->poll_fds.empty()) {
        print("No pollfds");
    }
    print("Current pollfds");
    for (pollfd &fd : this->poll_fds) {
        print("POLLFD FD: " + std::to_string(fd.fd) +
              ", EVENTS: " + std::to_string(fd.events));
    }
}

Hub::~Hub() {
    logWarn("Dropping Hub");
    delete this->socket;
}
