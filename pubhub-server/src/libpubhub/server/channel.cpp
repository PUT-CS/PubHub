#include "channel.hpp"
#include <string>

void Channel::addSubscriber(ClientId id) noexcept { subscribers.insert(id); }

void Channel::removeSubscriber(ClientId id) noexcept { subscribers.erase(id); }

void Channel::setName(ChannelName channel_name) {
    this->name = channel_name;
}

Channel::Channel() { this->id = this->channel_id_gen++; }

Channel::Channel(std::string name) {
    this->name = name;
    this->id = this->channel_id_gen++;
}

Channel::~Channel() {}

