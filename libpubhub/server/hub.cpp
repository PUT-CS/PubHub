#include "hub.hpp"
#include "../net/ServerSocket.hpp"
#include "../net/exceptions.hpp"
#include "client.hpp"
#include "event.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>
#include <bits/types/time_t.h>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <sys/poll.h>
#include <utility>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    pollfd server_pollfd = {this->socket->fd,
                            Hub::POLL_INPUT | Hub::POLL_ERROR, 0};
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
    pollfd poll_fd = {client.getFd(), Hub::POLL_ERROR | Hub::POLL_INPUT, 0};
    this->poll_fds.push_back(poll_fd);

    // Add Client to the general Client vec
    this->clients.insert({client.getFd(), client});
}

void Hub::removeClientByFd(int fd) {
    auto client_opt = this->clientByFd(fd);
    if (!client_opt.has_value()) {
        throw ClientException("No client with FD = " + std::to_string(fd));
    }
    Client &client = client_opt->get();

    client.killConnection();

    // Erase the client from the standard list of clients
    this->clients.erase(client.getFd());

    // Erase the client's pollfd
    std::erase_if(this->poll_fds, [fd](pollfd &pfd) { return pfd.fd == fd; });
}

auto Hub::clientByFd(int fd) -> std::optional<std::reference_wrapper<Client>> {
    auto found = this->clients.find(fd);

    if (found == this->clients.end()) {
        return std::nullopt;
    }
    return std::reference_wrapper<Client>(found->second);
}

void Hub::debugLogClients() {
    if (this->clients.empty()) {
        print("No Clients");
        return;
    }
    print("Current Clients:");
    for (auto &client : this->clients) {
        print(client.second.fmt());
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
