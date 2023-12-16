#pragma once

#ifndef CLIENT_H
#define CLIENT_H

#include <mutex>
#include <cstdint>
#include <optional>
#include "../net/Socket.hpp"
#include "request.hpp"
#include "nlohmann/json.hpp"
#include "response.hpp"
#include <set>

class Client {
  public:
    /// Socket used for exchange of requests and responses
    ClientSocket socket;
    
    /// Socket for sending messages from subscribed channels.
    /// Initialized when accepting the client, but separately form the constructor.
    ClientSocket broadcast_socket{};
    std::set<ChannelId> subscriptions = {};

    std::mutex* lock;

    Client();
    Client(ClientSocket);
    
    void initializeBroadcast(uint16_t);
    void subscribeTo(ChannelId) noexcept;
    void unsubscribeFrom(ChannelId) noexcept;
    FileDescriptor getFd() const noexcept;
    void killConnection() noexcept;
    nlohmann::json receiveMessage();
    void sendResponse(const Response&);
    void publishMessage(nlohmann::json);
    void setListenForWrite(bool);
    std::string fmt() const noexcept;

    ~Client();
};

#endif
