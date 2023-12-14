#ifndef STATE_CONTROLLER_HPP
#define STATE_CONTROLLER_HPP

#include "channel.hpp"
#include "client.hpp"
#include "event.hpp"
#include "types.hpp"
#include "../rustex.h"
#include <mutex>
#include <shared_mutex>
#include <sys/poll.h>
#include <unordered_map>
#include <vector>

using namespace rustex;

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
    Event nextEvent();
    void registerPollFdFor(FileDescriptor) noexcept;

    auto getClients() noexcept -> std::unordered_map<ClientId, Client> &;
    void addClient(Client) noexcept;
    void removeClientByFd(Client&) noexcept;
    auto clientByFd(FileDescriptor) -> Client &;
    void clearEventsByFd(FileDescriptor);
    void setPollingByFd(FileDescriptor, bool);

    //void setListenForWrite(ClientId, bool) noexcept;
    //auto lockClient(ClientId) -> std::lock_guard<std::mutex>;

    void addSubscription(ClientId, ChannelName);
    void removeSubscription(ClientId, ChannelName);

    auto getChannels() noexcept -> std::unordered_map<ChannelId, Channel> &;
    auto channelIdByName(ChannelName) -> ChannelId;
    auto channelById(ChannelId) -> Channel &;
    void addChannel(ChannelName);
    void deleteChannel(ChannelName);

    bool channelExists(const ChannelName &) const noexcept;

    void debugLogClients() const noexcept;
    void debugLogPollFds() const noexcept;
    void debugLogChannels() const noexcept;
};

#endif
