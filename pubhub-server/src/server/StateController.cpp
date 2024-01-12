#include "StateController.hpp"
#include "../common.hpp"
#include "../net/exceptions.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "Event.hpp"
#include "exceptions.hpp"
#include "types.hpp"
#include <algorithm>
#include <functional>
#include <set>
#include <string>
#include <sys/poll.h>
#include <unordered_map>
#include <utility>

/**
   Wait for the next event on either the server or one of the clients.

   Throws:
   - **NetworkException** if `poll` fails.
 **/
auto StateController::nextEvent() -> Event {
    int n_of_events = poll(this->poll_fds.data(), this->poll_fds.size(), -1);

    if (n_of_events == -1) {
        throw NetworkException("Poll");
    }

    // Handle a server input event
    if (poll_fds[0].revents & StateController::POLL_INPUT) {
        return Event{EventKind::ConnectionRequest, poll_fds[0].fd};
    }

    // Look for a client with an event
    for (uint64_t i = 1; i < clients.size() + 1; i++) {
        const auto pfd = &this->poll_fds[i];

        if (!pfd->revents) {
            continue;
        }

        // Connection to the client was distrupted for some reason
        if (pfd->revents & StateController::POLL_ERROR) {
            return Event{EventKind::Disconnect, pfd->fd};
        }

        // Client sent data and it's ready to read
        if (pfd->revents & StateController::POLL_INPUT) {
            return Event{EventKind::Input, pfd->fd};
        }
    }
    // just so it compiles with returns on all paths
    ERROR("Unreachable");
    return Event{EventKind::Nil, -1};
}

void StateController::registerPollFdFor(FileDescriptor fd) noexcept {
    pollfd pfd = {fd, StateController::POLL_INPUT | StateController::POLL_ERROR,
                  0};
    this->poll_fds.push_back(pfd);
}

auto StateController::getClients() noexcept
    -> std::unordered_map<ClientId, Client> & {
    return std::ref(this->clients);
}

void StateController::addClient(Client client) noexcept {
    // Add Client to the pollfd vector for polling events
    this->registerPollFdFor(client.getFd());

    // Add Client to the general Client vec
    this->clients.insert({client.getFd(), client});
}

void StateController::removeClientByFd(Client &client) noexcept {
    client.killConnection();

    // Remove all subscriptions of that client
    for (auto &sub_id : client.getSubscriptions()) {
        this->channels[sub_id].removeSubscriber(client.getFd());
    }

    // Erase the client from the standard list of clients
    this->clients.erase(client.getFd());

    // Erase the client's pollfd
    std::erase_if(this->poll_fds,
                  [&](pollfd &pfd) { return pfd.fd == client.getFd(); });
}

/// Assumes the client exists
auto StateController::clientByFd(int fd) noexcept -> Client & {
    return std::ref(this->clients.find(fd)->second);
}

// void StateController::setListenForWrite(ClientId id, bool status) noexcept {
//     short new_flags = (status ? POLLOUT : 0) | StateController::POLL_ERROR |
//                      StateController::POLL_INPUT;
//     pollfd new_pfd = {id, new_flags, 0};

//     auto it = std::find_if(this->poll_fds.begin(), this->poll_fds.end(),
//                  [&](pollfd pfd) { return pfd.fd == id; });
//     int idx = std::distance(this->poll_fds.begin(), it);

//     this->poll_fds[idx] = new_pfd;
// }

// Lock a client, disable listening for input
// std::lock_guard<std::mutex> StateController::lockClient(ClientId id) {
//     return std::lock_guard<std::mutex>(*this->clients.at(id).lock);
// }

void StateController::setPollingByFd(FileDescriptor fd, bool enable) {
    auto it = std::find_if(poll_fds.begin(), poll_fds.end(),
                           [&](pollfd pfd) { return abs(pfd.fd) == fd; });
    // Don't do anything if the client has been erased already
    if (it == this->poll_fds.end()) {
        ERROR("Client with FD = " + std::to_string(fd) + " not found");
        return;
    }
    it->events =
        enable ? StateController::POLL_INPUT | StateController::POLL_ERROR : 0;
}

/**
   Throws:
   - **ClientException** if no Client with passed id exists
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
void StateController::addSubscription(ClientId client_id,
                                      ChannelName channel_name) {
    auto channel_id = this->channelIdByName(channel_name);

    this->channels.at(channel_id).addSubscriber(client_id);
    this->clients.at(client_id).subscribeTo(channel_id);
}

/**
   Throws:
   - **ClientNotFoundException** if no Client with passed id exists
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
void StateController::removeSubscription(ClientId client_id,
                                         ChannelName channel_name) {
    auto channel_id = this->channelIdByName(channel_name);
    this->channels[channel_id].removeSubscriber(client_id);
    this->clients[client_id].unsubscribeFrom(channel_id);
}

auto StateController::getChannels() noexcept
    -> std::unordered_map<ChannelId, Channel> & {
    return std::ref(this->channels);
}

/**
   Throws:
   - **ChannelNotFoundException** if no channel with passed name exists
 **/
auto StateController::channelIdByName(ChannelName channel_name) -> ChannelId {
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
void StateController::addChannel(ChannelName channel_name) {
    if (this->channelExists(channel_name)) {
        throw ChannelAlreadyExistsException("Channel named " + channel_name +
                                            " already exists");
    }
    if (string_has_whitespace(channel_name)) {
        throw InvalidInputException("Channel names cannot contain whitespace");
    }
    auto channel = Channel(channel_name);
    this->channels.insert({channel.id, channel});
}

/**
   Throws:
   - **ChannelNotFoundException** if the channel already doesn't exist
 **/
void StateController::deleteChannel(ChannelName channel_name) {
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
auto StateController::channelById(ChannelId id) -> Channel & {
    auto found = this->channels.find(id);

    if (found == this->channels.end()) {
        throw ChannelNotFoundException("No Channel with ID = " +
                                       std::to_string(id));
    }

    return std::ref(found->second);
}

auto StateController::channelExists(const ChannelName &name) const noexcept -> bool {
    auto f = [&](const std::pair<ChannelId, Channel> &p) {
        return p.second.name == name;
    };

    auto found = std::find_if(channels.begin(), channels.end(), f);
    return found != channels.end();
}

void StateController::debugLogClients() const noexcept {
    if (this->clients.empty()) {
        DEBUG("No Clients");
        return;
    }
    DEBUG("Current Clients:");
    for (const auto &client : this->clients) {
        DEBUG(client.second.fmt());
    }
}

void StateController::debugLogPollFds() const noexcept {
    if (this->poll_fds.empty()) {
        DEBUG("No pollfds");
        return;
    }
    DEBUG("Current pollfds");
    for (const pollfd &fd : this->poll_fds) {
        DEBUG("POLLFD FD: " + std::to_string(fd.fd) +
              ", EVENTS: " + std::to_string(fd.events));
    }
}

void StateController::debugLogChannels() const noexcept {
    if (this->channels.empty()) {
        DEBUG("No channels");
        return;
    }
    DEBUG("Current Channels:");
    for (const auto &[id, channel] : this->channels) {
        DEBUG("ID: " + std::to_string(id) + ", Name: " + channel.name +
              ", Subscribers: " + std::to_string(channel.subscribers.size()));
    }
}
