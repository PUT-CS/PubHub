#pragma once
#ifndef QUEUE_H
#define QUEUE_H

#include "client.hpp"
#include "message.hpp"
#include "types.hpp"
#include <optional>
#include <set>
#include <unordered_map>
#include <vector>


class Channel {
    public:
    static ChannelId channel_id_gen;
    ChannelName name;
    std::set<ClientId> subscribers;
    ChannelId id;
    
    Channel();
    ~Channel();
    void setName(ChannelName);
    void addSubscriber(ClientId) noexcept;
    void removeSubscriber(ClientId) noexcept;
};

#endif
