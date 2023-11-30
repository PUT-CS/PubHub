#pragma once
#ifndef HUB_H
#define HUB_H

#include "event.hpp"
#include "channel.hpp"
#include "sys/poll.h"
#include "../common.hpp"

class Hub {
  private:
    /**
       Holds all pollfds needed to check for socket activity.
       The first pollfd in this vector shall always be the server pollfd.
       Every time a client connects or disconnects this has to be updated
     **/
    std::vector<pollfd> poll_fds;
    std::map<std::string, PayloadKind> PayloadKind_map;
  public:
    static const auto POLL_ERROR = POLLERR | POLLNVAL | POLLHUP | POLLRDHUP;
    static const auto POLL_INPUT = POLLIN;

    /// ClientId is the Client's file descriptor
    std::unordered_map<FileDescriptor, Client> clients;
    std::unordered_map<ChannelId, Channel> channels;
    
    SocketAddress addr;
    std::unique_ptr<ServerSocket> socket;
    

    Hub(SocketAddress);
    Event nextEvent(time_t);
    void listen();
    Client accept();

    void addClient(Client) noexcept;
    void removeClientByFd(FileDescriptor);
    auto clientByFd(FileDescriptor) -> Client&;

    std::map<std::string, PayloadKind> getPayloadKind_map();
    void setPayloadKind_map();
    
    auto channelIdByName(ChannelName) -> ChannelId;
    void addSubscription(ClientId, ChannelName);
    void removeSubscription(ClientId, ChannelName);

    void debugLogClients();
    void debugLogPollFds();

    ~Hub();
};

#endif
