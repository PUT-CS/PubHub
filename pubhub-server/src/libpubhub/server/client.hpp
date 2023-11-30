#pragma once
#ifndef CLIENT_H
#define CLIENT_H
#include "../net/Socket.hpp"
#include "message.hpp"
#include <set>

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
    nlohmann::json receiveMessage();
    void sendMessage(const Payload&);
    std::string fmt();

    ~Client();
};

#endif
