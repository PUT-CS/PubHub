#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "queue.hpp"
#include <algorithm>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <utility>
#include <vector>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = new ServerSocket(addr);
    this->socket->bind();
    this->socket->listen();

    pollfd server_pollfd = {this->socket->fd, POLLIN | POLLHUP | POLLERR, 0};
    this->poll_fds.push_back(server_pollfd);
    logInfo("Registered server pollfd");
}

void Hub::run() {
    while (true) {
        //this->debugLogClients();
        this->handleNextEvent();
        // this->accept();
        // logInfo("Ready to accept next Client");
    }
}

/**
   Wait for the next event on either the server or one of the clients
 **/
void Hub::handleNextEvent() {
    //logError("BEGIN HANDLE");
    // blocks
    int n_of_events = poll(this->poll_fds.data(), this->poll_fds.size(), -1);
    if (n_of_events == -1) {
        perror("Poll");
        return;
    }

    // Handle a server input event
    if (poll_fds[0].revents & POLLIN) {
        this->accept();
        return;
    }

    // Look for a client with an event
    for (long unsigned int i = 1; i < clients.size() + 1; i++) {
        auto pfd = &this->poll_fds[i];
        std::string cfd = std::to_string(pfd->fd);

        // Connection to the client was distrupted for some reason
        if (pfd->revents & (POLLERR | POLLNVAL | POLLHUP)) {
            logWarn("Client disconnected: " + cfd);
            this->removeClientByFd(pfd->fd);
            return;
        }

        // Client sent data and it's ready to read
        if (pfd->revents & POLLIN) {
            char buf[16] = {};
            int n = recv(pfd->fd, buf, 16, 0);
            if (n == -1) {
                perror("Recv");
            }
            auto msg = std::string(buf);
            if (msg.ends_with('\n')) {
                msg.pop_back();
            }
            logInfo("Received from " + cfd + ": " + msg);
        }
    }
}

void Hub::accept() {
    auto client_socket = this->socket->accept().unwrap();
    auto client = Client(client_socket);

    this->addClient(client);
    logInfo("Added Client: " + client.fmt());
}

void Hub::addClient(Client client) {
    // Add Client to the pollfd vector for polling events
    pollfd poll_fd = {client.getFd(), POLLIN | POLLHUP | POLLERR, 0};
    this->poll_fds.push_back(poll_fd);

    // Add Client to the general Client vec
    this->clients.push_back(std::make_pair(client, std::vector<QueuePtr>{}));
}

void Hub::removeClientByFd(int fd) {
    auto client = this->clientByFd(fd);
    if (!client.has_value()) {
        throw std::runtime_error("No client with FD = " + std::to_string(fd));
    }
    client.value().get()->killConnection();
    
    // Erase the client from the standard list of clients
    std::erase_if(this->clients, [=](auto& pair) {
        return pair.first.getFd() == fd;
    });
    // Erase the client's pollfd
    std::erase_if(this->poll_fds, [=](pollfd& pfd) {
        return pfd.fd == fd;
    });
}

auto Hub::clientByFd(int fd) -> std::optional<std::shared_ptr<Client>> {
    auto it = std::find_if(clients.begin(), clients.end(), [&](auto &pair) {
        return pair.first.socket.fd == fd;
    });
    
    if (it == this->clients.end()) {
        std::cout << "Element 5 not found in the vector." << std::endl;
        return std::nullopt;
    }
    
    int idx = std::distance(clients.begin(), it);
    Client c = clients[idx].first;
    auto ptr = std::make_shared<Client>(c);
    return std::make_optional(ptr);
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
