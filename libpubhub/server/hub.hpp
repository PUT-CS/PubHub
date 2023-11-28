#ifndef HUB_H
#define HUB_H

#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "event.hpp"
#include "message.hpp"
#include "channel.hpp"
#include "sys/poll.h"
#include "types.hpp"
#include <bits/types/time_t.h>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <sys/poll.h>
#include <sys/types.h>
#include <unordered_map>
#include <vector>

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
    
    void addSubscription(ClientId, ChannelName);
    void removeSubscription(ClientId, ChannelName);

    auto channelIdByName(ChannelName) -> ChannelId;
    auto channelById(ChannelId) -> Channel &;
    void addChannel(ChannelName);
    void deleteChannel(ChannelName);
    
    void debugLogClients();
    void debugLogPollFds();

    ~Hub();
};

#endif
