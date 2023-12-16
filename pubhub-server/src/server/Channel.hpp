#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include "types.hpp"
#include <set>
#include <string>

class Channel {
  private:
    static inline ChannelId idGen = 0;

  public:
    ChannelName name;
    std::set<ClientId> subscribers;
    ChannelId id;

    Channel();
    explicit Channel(std::string);

    void addSubscriber(ClientId) noexcept;
    void removeSubscriber(ClientId) noexcept;

    ~Channel();
};

#endif
