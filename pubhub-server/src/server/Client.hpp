#pragma once

#include "types.hpp"
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
    ClientSocket broadcast_socket{};
    std::set<ChannelId> subscriptions = {};

  public:
    Client();
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
    void sendResponse(const Response &);
    void publishMessage(const std::string&);
    void setListenForWrite(bool);
    [[nodiscard]] auto fmt() const noexcept -> std::string;

    ~Client();
};

#endif
