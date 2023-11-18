#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "../net/exceptions.hpp"
#include "client.hpp"
#include "event.hpp"
#include "exceptions.hpp"
#include <algorithm>
#include <bits/types/time_t.h>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <string>
#include <sys/poll.h>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    pollfd server_pollfd = {this->socket->fd,
                            POLLIN | POLLHUP | POLLERR | POLLNVAL, 0};
    this->poll_fds.push_back(server_pollfd);
}

/**
   Wait for the next event on either the server or one of the clients.

   Throws NetworkException if `poll` fails.
 **/
Event Hub::nextEvent(time_t timeout) {
    int n_of_events =
        poll(this->poll_fds.data(), this->poll_fds.size(), timeout);

    if (n_of_events == -1) {
        throw NetworkException("Poll");
    }

    // Handle a server input event
    if (poll_fds[0].revents & Hub::POLL_INPUT) {
        return Event{EventKind::ConnectionRequest, poll_fds[0].fd};
    }

    // Look for a client with an event
    for (long unsigned int i = 1; i < clients.size() + 1; i++) {
        const auto pfd = &this->poll_fds[i];

        if (!pfd->revents) {
            continue;
        }

        // Connection to the client was distrupted for some reason
        if (pfd->revents & Hub::POLL_ERROR) {
            return Event{EventKind::Disconnect, pfd->fd};
        }

        // Client sent data and it's ready to read
        if (pfd->revents & Hub::POLL_INPUT) {
            return Event{EventKind::Input, pfd->fd};
        }
    }
    return Event{EventKind::Nil};
}

Client Hub::accept() {
    auto client_socket = this->socket->accept();
    return Client(client_socket);
}

void Hub::addClient(Client client) noexcept {
    // Add Client to the pollfd vector for polling events
    pollfd poll_fd = {client.getFd(), POLLIN | POLLHUP | POLLERR, 0};
    this->poll_fds.push_back(poll_fd);

    // Add Client to the general Client vec
    this->clients.push_back(std::make_pair(client, std::vector<QueuePtr>{}));
}

void Hub::removeClientByFd(int fd) {
    auto client = this->clientByFd(fd);
    if (!client.has_value()) {
        throw ClientException("No client with FD = " + std::to_string(fd));
    }

    client.value().get()->killConnection();
 
    // Erase the client from the standard list of clients
    std::erase_if(this->clients,
                  [=](auto &pair) { return pair.first.getFd() == fd; });
    // Erase the client's pollfd
    std::erase_if(this->poll_fds, [=](pollfd &pfd) { return pfd.fd == fd; });
}

auto Hub::clientByFd(int fd) -> std::optional<std::shared_ptr<Client>> {
    auto it = std::find_if(clients.begin(), clients.end(), [&](auto &pair) {
        return pair.first.socket.fd == fd;
    });

    if (it == this->clients.end()) {
        return std::nullopt;
    }

    int idx = std::distance(clients.begin(), it);
    auto ptr = std::make_shared<Client>(clients[idx].first);
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

Hub::~Hub() { logWarn("Dropping Hub"); }
