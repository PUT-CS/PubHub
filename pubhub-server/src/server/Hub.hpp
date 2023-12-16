#pragma once
#ifndef HUB_H
#define HUB_H

#include "Client.hpp"
#include "Request.hpp"
#include "StateController.hpp"
#include "../common.hpp"
#include "Channel.hpp"
#include "Event.hpp"

class Hub {
  private:
    std::unique_ptr<ServerSocket> socket;
    StateController state_controller;

    void handleEvent(Event);
    void handleInput(FileDescriptor);
    void handleDisconnect(FileDescriptor fd) noexcept;
    void handleNewConnection() noexcept;

    auto handleSubscribe(const Client& client, const ChannelName& target) -> Response;
    auto handleUnsubscribe(const Client& client, const ChannelName& target) -> Response;
    auto handleCreateChannel(const ChannelName& target) -> Response;
    auto handleDeleteChannel(const ChannelName& target) -> Response;
    auto handlePublish(const ChannelName& target, const nlohmann::json& content) -> Response;
    auto handleAsk() -> Response;
    
    auto accept() -> Client;

  public:
    explicit Hub(SocketAddress);
    [[noreturn]] void run();
    ~Hub();
};

#endif
