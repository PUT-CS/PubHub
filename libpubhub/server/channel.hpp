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
    ChannelName name;
    std::set<ClientId> subscribers;
    
    Channel();
    ~Channel();

    void addSubscriber(ClientId) noexcept;
    void removeSubscriber(ClientId) noexcept;
};

#endif
