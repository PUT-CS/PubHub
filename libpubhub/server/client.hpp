#ifndef CLIENT_H
#define CLIENT_H
#include "../net/ClientSocket.hpp"
#include "message.hpp"
#include "types.hpp"
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>

class Client {
  public:
    ClientSocket socket;
    std::set<ChannelId> subscriptions;

    Client();
    Client(ClientSocket);
    
    void subscribeTo(ChannelId) noexcept;
    void unsubscribeFrom(ChannelId) noexcept;
    FileDescriptor getFd();
    void killConnection() noexcept;
    
    std::string fmt();

    ~Client();
};

#endif
