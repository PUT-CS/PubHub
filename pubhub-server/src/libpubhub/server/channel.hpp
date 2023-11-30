#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#include "client.hpp"

class Channel {
    public:
    ChannelName name;
    std::set<ClientId> subscribers;
    
    Channel();
    ~Channel();

    void addSubscriber(ClientId) noexcept;
    void removeSubscriber(ClientId) noexcept;
};

#endif
