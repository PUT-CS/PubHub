#pragma once
#ifndef HUB_H
#define HUB_H

#include "client.hpp"
#include "request.hpp"
#include "state_controller.hpp"
#include "types.hpp"
#include <functional>
#include "../common.hpp"
#include "channel.hpp"
#include "event.hpp"
#include "sys/poll.h"

class Hub {
  private:
    typedef std::function<Response ()> HandlerFn;

    std::unique_ptr<ServerSocket> socket;
    StateController state_controller;
    

    void handleEvent(Event);
    void handleInput(FileDescriptor);
    void handleDisconnect(FileDescriptor fd) noexcept;
    void handleNewConnection() noexcept;

    Response handleSubscribe(const Client& client, const ChannelName& target);
    Response handleUnsubscribe(const Client& client, const ChannelName& target);
    Response handleCreateChannel(const ChannelName& target);
    Response handleDeleteChannel(const ChannelName& target);
    Response handlePublish(const ChannelName& target, const nlohmann::json& content);
    Response handleAsk();
    
    Client accept();

  public:
    Hub(SocketAddress);
    void run();
    ~Hub();
};

#endif
