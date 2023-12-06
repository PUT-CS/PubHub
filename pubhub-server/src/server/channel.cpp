#include "channel.hpp"

void Channel::addSubscriber(ClientId id) noexcept { subscribers.insert(id); }

void Channel::removeSubscriber(ClientId id) noexcept { subscribers.erase(id); }

Channel::Channel() { this->id = this->idGen++; }

Channel::Channel(std::string name) {
    this->name = name;
    this->id = this->idGen++;
}

Channel::~Channel() {}
