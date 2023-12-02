#pragma once
#include <string>
#ifndef QUEUE_H
#define QUEUE_H

#include "client.hpp"

class Channel {
    public:
    static ChannelId channel_id_gen;
    ChannelName name;
    std::set<ClientId> subscribers;
    ChannelId id;
    
    Channel();
    Channel(std::string);
    
    void setName(ChannelName);
    void addSubscriber(ClientId) noexcept;
    void removeSubscriber(ClientId) noexcept;
    
    ~Channel();
};

#endif
