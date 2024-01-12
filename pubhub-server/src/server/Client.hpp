#pragma once

#include "types.hpp"
#include <cstddef>
#include <deque>
#include <string>
#ifndef CLIENT_H
#define CLIENT_H

#include "../json.hpp"
#include "../net/Socket.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <optional>
#include <set>

class Client {
  private:
    /// Socket used for exchange of requests and responses
    ClientSocket socket;

    /// Socket for sending messages from subscribed channels.
    /// Initialized when accepting the client, but separately form the
    /// constructor.
    //ClientSocket broadcast_socket{};
    std::set<ChannelId> subscriptions = {};

    // messages that the client should receive
    std::deque<std::string> backlog_queue = {};

  public:
    Client() = default;
    
    explicit Client(ClientSocket);

    auto getSocket() -> ClientSocket &;
    auto getBroadcastSocket() -> ClientSocket &;
    auto getSubscriptions() -> const std::set<ChannelId> &;

    void initializeBroadcast(uint16_t);
    void subscribeTo(ChannelId) noexcept;
    void unsubscribeFrom(ChannelId) noexcept;
    [[nodiscard]] auto getFd() const noexcept -> FileDescriptor;
    void killConnection() noexcept;
    [[nodiscard]] auto receiveMessage() -> nlohmann::json;
    void enqueueMessage(const std::string& msg);
    void flushOne();
    auto hasBacklog() -> bool;
    auto backlogSize() -> size_t;
    //void sendResponse(const Response &);
    //void publishMessage(const std::string&);
    void setListenForWrite(bool);
    [[nodiscard]] auto fmt() const noexcept -> std::string;

    ~Client() = default;
};

#endif
