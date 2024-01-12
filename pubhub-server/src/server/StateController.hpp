#ifndef STATE_CONTROLLER_HPP
#define STATE_CONTROLLER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Event.hpp"
#include "types.hpp"
#include <random>
#include <sys/poll.h>
#include <unordered_map>
#include <vector>

class StateController {
  private:
    /**
   Holds all pollfds needed to check for socket activity.
   The first pollfd in this vector shall always be the server pollfd.
   Every time a client connects or disconnects this has to be updated
 **/
    std::vector<pollfd> poll_fds;

    std::unordered_map<FileDescriptor, Client> clients;
    std::unordered_map<ChannelId, Channel> channels;

    std::random_device _rd;
    std::default_random_engine _rng;

    static const auto POLL_ERROR = POLLERR | POLLNVAL | POLLHUP | POLLRDHUP;
    static const auto POLL_INPUT = POLLIN;
    static const auto POLL_OUTPUT = POLLOUT;

    static const auto POLL_ALL = POLL_ERROR | POLL_INPUT | POLL_OUTPUT;

  public:
    auto nextEvent() -> Event;
    void registerPollFdFor(FileDescriptor) noexcept;

    auto getClients() noexcept -> std::unordered_map<ClientId, Client> &;
    void addClient(Client&&) noexcept;
    void removeClientByFd(Client &) noexcept;
    auto clientByFd(FileDescriptor) noexcept -> Client &;
    void setPollOutByFd(FileDescriptor, bool);

    void addSubscription(ClientId, const ChannelName&);
    void removeSubscription(ClientId, const ChannelName&);

    auto getChannels() noexcept -> std::unordered_map<ChannelId, Channel> &;
    auto channelIdByName(const ChannelName&) -> ChannelId;
    auto channelById(ChannelId) -> Channel &;
    void addChannel(const ChannelName&);
    void deleteChannel(const ChannelName&);

    auto channelExists(const ChannelName &) const noexcept -> bool;

    void debugLogClients() const noexcept;
    void debugLogPollFds() const noexcept;
    void debugLogChannels() const noexcept;
};

#endif
