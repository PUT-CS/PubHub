#include "hub.hpp"
#include "../net/exceptions.hpp"
#include "exceptions.hpp"

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
   - **NetworkException** if accept fails
**/
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

/**
   Throws:
   - **ClientException** if there's no Client with passed file descriptor (ID)
 **/
void Hub::removeClientByFd(int fd) {
    auto client = this->clientByFd(fd);

    client.killConnection();
    std::set<Client> c{};

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
        throw ClientNotFoundException("No Client with ID = " + std::to_string(fd));
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
    auto channel = Channel();
    channel.setName(channel_name);

    // auto found = this->channels.find(channelIdByName(channel_name));

    // if (found == this->clients.end()) {
    //     throw ChannelAlreadyExists("No Client with ID = " + std::to_string(id));
    // }
    
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
        throw ChannelNotFoundException("No Channel with ID = " + std::to_string(id));
    }

    return std::ref(found->second);
}

void Hub::debugLogClients() noexcept {
    if (this->clients.empty()) {
        print("No Clients");
        return;
    }
    print("Current Clients:");
    for (auto &client : this->clients) {
        print(client.second.fmt());
    }
}

void Hub::debugLogPollFds() noexcept {
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
