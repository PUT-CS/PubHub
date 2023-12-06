#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "channel.hpp"
#include "client.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>
#include <string>
#include <unordered_map>
#include <utility>

/// Create the Hub. Initializes the server socket and the first pollfd.
Hub::Hub(SocketAddress addr) {
    this->socket = std::make_unique<ServerSocket>(addr);
    this->socket->bind();
    this->socket->listen();

    pollfd server_pollfd = {this->socket->fd, Hub::POLL_INPUT | Hub::POLL_ERROR,
                            0};
    this->poll_fds.push_back(server_pollfd);
}

/**
   Wait for the next event on either the server or one of the clients.

   Throws:
   - **NetworkException** if `poll` fails.
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
    return Event{EventKind::Nil, -1};
}

/**
   Throws:
   - **NetworkException** if either accept() or connect() fail
**/
Client Hub::accept() {
    auto client_socket = this->socket->accept();
    auto client = Client(client_socket);
    
    auto hub_port = this->socket->address().getPort();
    client.initializeBroadcast(hub_port + 1);
    
    return client;
}

void Hub::addClient(Client client) noexcept {
    // Add Client to the pollfd vector for polling events
    pollfd poll_fd = {client.getFd(), Hub::POLL_ERROR | Hub::POLL_INPUT, 0};
    this->poll_fds.push_back(poll_fd);

    // Add Client to the general Client vec
    this->clients.insert({client.getFd(), client});
}

/**
   Throws:
   - **ClientException** if there's no Client with passed file descriptor (ID)
 **/
void Hub::removeClientByFd(int fd) {
    auto client = this->clientByFd(fd);

    client.killConnection();
    
    // Remove all subscriptions of that client
    for (auto &sub_id : client.subscriptions) {
        this->channels[sub_id].removeSubscriber(client.getFd());
    }

    // Erase the client from the standard list of clients
    this->clients.erase(client.getFd());

    // Erase the client's pollfd
    std::erase_if(this->poll_fds, [fd](pollfd &pfd) { return pfd.fd == fd; });
}

/**
   Throws:
    - **ClientNotFoundException** if no Client with that Id exists.
 **/
auto Hub::clientByFd(int fd) -> Client & {
    auto found = this->clients.find(fd);

    if (found == this->clients.end()) {
        throw ClientNotFoundException("No Client with ID = " +
                                      std::to_string(fd));
    }

    return std::ref(found->second);
}

/**
   Throws:
   - **ClientException** if no Client with passed id exists
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
void Hub::addSubscription(ClientId client_id, ChannelName channel_name) {
    auto client = this->clientByFd(client_id);
    auto channel_id = this->channelIdByName(channel_name);

    this->channels.at(channel_id).addSubscriber(client_id);
    this->clients.at(client_id).subscribeTo(channel_id);
}

/**
   Throws:
   - **ClientNotFoundException** if no Client with passed id exists
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
void Hub::removeSubscription(ClientId client_id, ChannelName channel_name) {
    auto client = this->clientByFd(client_id);
    auto channel_id = this->channelIdByName(channel_name);

    this->channels[channel_id].removeSubscriber(client_id);
    this->clients[client_id].unsubscribeFrom(channel_id);
}

/**
   Throws:
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
ChannelId Hub::channelIdByName(ChannelName channel_name) {
    for (auto &[id, channel] : this->channels) {
        if (channel.name == channel_name) {
            return id;
        }
    }
    throw ChannelNotFoundException("No Channel named `" + channel_name + '`');
}

/**
   Throws:
   - **ChannelAlreadyCreated** if channel with passed name already exists
 **/
void Hub::addChannel(ChannelName channel_name) {
    if (this->channelExists(channel_name)) {
        throw ChannelAlreadyExistsException("Channel named " + channel_name +
                                            " already exists");
    }
    auto channel = Channel(channel_name);
    this->channels.insert({channel.id, channel});
}

/**
   Throws:
   - **ChannelNotFoundException** if the channel already doesn't exist
 **/
void Hub::deleteChannel(ChannelName channel_name) {
    auto id = this->channelIdByName(channel_name);
    auto channel = channelById(id);
    for (auto &sub_id : channel.subscribers) {
        this->clients[sub_id].unsubscribeFrom(id);
    }
    this->channels.erase(id);
}

/** Throws:
    - **ChannelNotFoundException** if the channel doesn't exists
 **/
auto Hub::channelById(ChannelId id) -> Channel & {
    auto found = this->channels.find(id);

    if (found == this->channels.end()) {
        throw ChannelNotFoundException("No Channel with ID = " +
                                       std::to_string(id));
    }

    return std::ref(found->second);
}

bool Hub::channelExists(const ChannelName &name) const noexcept {
    auto f = [&](const std::pair<ChannelId, Channel> &p) {
        return p.second.name == name;
    };

    auto found = std::find_if(channels.begin(), channels.end(), f);
    return found != channels.end();
}

void Hub::debugLogClients() const noexcept {
    if (this->clients.empty()) {
        print("No Clients");
        return;
    }
    print("Current Clients:");
    for (const auto &client : this->clients) {
        print(client.second.fmt());
    }
}

void Hub::debugLogPollFds() const noexcept {
    if (this->poll_fds.empty()) {
        print("No pollfds");
        return;
    }
    print("Current pollfds");
    for (const pollfd &fd : this->poll_fds) {
        print("POLLFD FD: " + std::to_string(fd.fd) +
              ", EVENTS: " + std::to_string(fd.events));
    }
}

void Hub::debugLogChannels() const noexcept {
    if (this->channels.empty()) {
        logDebug("No channels");
        return;
    }
    print("Current Channels:");
    for (const auto &[id, channel] : this->channels) {
        print("ID: " + std::to_string(id) + ", Name: " + channel.name +
              ", Subscribers: " + std::to_string(channel.subscribers.size()));
    }
}

Hub::~Hub() { logWarn("Dropping Hub"); }
