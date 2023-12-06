#pragma once
#include "client.hpp"
#include "request.hpp"
#include "types.hpp"
#include <functional>
#ifndef HUB_H
#define HUB_H

#include "../common.hpp"
#include "channel.hpp"
#include "event.hpp"
#include "sys/poll.h"

class Hub {
  private:
    
    /**
       Holds all pollfds needed to check for socket activity.
       The first pollfd in this vector shall always be the server pollfd.
       Every time a client connects or disconnects this has to be updated
     **/
    std::vector<pollfd> poll_fds;

  public:
    typedef std::function<void ()> HandlerFn;
    
    static const auto POLL_ERROR = POLLERR | POLLNVAL | POLLHUP | POLLRDHUP;
    static const auto POLL_INPUT = POLLIN;

    std::unordered_map<FileDescriptor, Client> clients;
    std::unordered_map<ChannelId, Channel> channels;

    SocketAddress addr;
    std::unique_ptr<ServerSocket> socket;

    
    Hub(SocketAddress);

    void run();
    void handleEvent(Event);
    void handleInput(FileDescriptor);
    void handleDisconnect(FileDescriptor fd);
    void handleNewConnection();

    HandlerFn subscribeHandler(const Client& client, const ChannelName& target);
    HandlerFn unsubscribeHandler(const Client& client, const ChannelName& target);
    HandlerFn createChannelHandler(const ChannelName& target);
    HandlerFn deleteChannelHandler(const ChannelName& target);
    HandlerFn publishHandler(const ChannelName& target, const std::string& content);
    
    Event nextEvent(time_t);
    Client accept();
    
    void addClient(Client) noexcept;
    void removeClientByFd(FileDescriptor);
    auto clientByFd(FileDescriptor) -> Client &;

    void addSubscription(ClientId, ChannelName);
    void removeSubscription(ClientId, ChannelName);

    auto channelIdByName(ChannelName) -> ChannelId;
    auto channelById(ChannelId) -> Channel &;
    void addChannel(ChannelName);
    void deleteChannel(ChannelName);

    bool channelExists(const ChannelName&) const noexcept;

    void debugLogClients() const noexcept;
    void debugLogPollFds() const noexcept;
    void debugLogChannels() const noexcept;

    ~Hub();
};

#endif
