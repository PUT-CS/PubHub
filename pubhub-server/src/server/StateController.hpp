#ifndef STATE_CONTROLLER_HPP
#define STATE_CONTROLLER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Event.hpp"
#include "types.hpp"
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

    static const auto POLL_ERROR = POLLERR | POLLNVAL | POLLHUP | POLLRDHUP;
    static const auto POLL_INPUT = POLLIN;

  public:
    auto nextEvent() -> Event;
    void registerPollFdFor(FileDescriptor) noexcept;

    auto getClients() noexcept -> std::unordered_map<ClientId, Client> &;
    void addClient(Client) noexcept;
    void removeClientByFd(Client &) noexcept;
    auto clientByFd(FileDescriptor) noexcept -> Client &;
    void setPollingByFd(FileDescriptor, bool);

    void addSubscription(ClientId, ChannelName);
    void removeSubscription(ClientId, ChannelName);

    auto getChannels() noexcept -> std::unordered_map<ChannelId, Channel> &;
    auto channelIdByName(ChannelName) -> ChannelId;
    auto channelById(ChannelId) -> Channel &;
    void addChannel(ChannelName);
    void deleteChannel(ChannelName);

    auto channelExists(const ChannelName &) const noexcept -> bool;

    void debugLogClients() const noexcept;
    void debugLogPollFds() const noexcept;
    void debugLogChannels() const noexcept;
};

#endif
