#pragma once
#include "client.hpp"
#include "request.hpp"
#include "state_controller.hpp"
#include "types.hpp"
#include <functional>
#ifndef HUB_H
#define HUB_H

#include "../common.hpp"
#include "channel.hpp"
#include "event.hpp"
#include "sys/poll.h"

class Hub {
  private:
    typedef std::function<void ()> HandlerFn;
    
    std::unique_ptr<ServerSocket> socket;
    StateController state_controller;
    

    void handleEvent(Event);
    void handleInput(FileDescriptor);
    void handleDisconnect(FileDescriptor fd);
    void handleNewConnection();

    HandlerFn subscribeHandler(const Client& client, const ChannelName& target);
    HandlerFn unsubscribeHandler(const Client& client, const ChannelName& target);
    HandlerFn createChannelHandler(const ChannelName& target);
    HandlerFn deleteChannelHandler(const ChannelName& target);
    HandlerFn publishHandler(const ChannelName& target, const std::string& content);
    HandlerFn askHandler();
    
    Client accept();

  public:
    Hub(SocketAddress);
    void run();
    ~Hub();
};

#endif
