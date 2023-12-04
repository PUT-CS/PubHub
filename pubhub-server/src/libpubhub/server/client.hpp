#pragma once
#include <cstdint>
#include <optional>
#ifndef CLIENT_H
#define CLIENT_H
#include "../net/Socket.hpp"
#include "message.hpp"
#include <set>

class Client {
  public:
    /// Socket used for exchange of requests and responses
    ClientSocket socket;
    
    /// Socket for sending messages from subscribed channels.
    /// Initialized when accepting the client, but separately form the constructor.
    ClientSocket broadcast_socket;
    std::set<ChannelId> subscriptions = {};

    Client();
    Client(ClientSocket);

    void initializeBroadcast(uint16_t);
    void subscribeTo(ChannelId) noexcept;
    void unsubscribeFrom(ChannelId) noexcept;
    FileDescriptor getFd() const noexcept;
    void killConnection() noexcept;
    nlohmann::json receiveMessage();
    void sendMessage(const Payload &);
    std::string fmt() const noexcept;

    ~Client();
};

#endif
