#pragma once
#include "message.hpp"
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

    // const std::map<std::string, PayloadKind> PayloadKind_map = {
    //     {"Subscribe", PayloadKind::Subscribe},
    //     {"Unsubscribe", PayloadKind::Unsubscribe},
    //     {"CreateChannel", PayloadKind::CreateChannel},
    //     {"DeleteChannel", PayloadKind::DeleteChannel},
    //     {"Publish", PayloadKind::Publish}
    // };

  public:
    typedef std::function<void ()> HandlerFn;
    
    static const auto POLL_ERROR = POLLERR | POLLNVAL | POLLHUP | POLLRDHUP;
    static const auto POLL_INPUT = POLLIN;

    /// ClientId is the Client's file descriptor
    std::unordered_map<FileDescriptor, Client> clients;
    std::unordered_map<ChannelId, Channel> channels;

    SocketAddress addr;
    std::unique_ptr<ServerSocket> socket;

    Hub(SocketAddress);
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

    void debugLogClients() noexcept;
    void debugLogPollFds() noexcept;

    ~Hub();
};

#endif
