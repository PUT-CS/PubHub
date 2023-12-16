#include "Channel.hpp"

#include <utility>

void Channel::addSubscriber(ClientId id) noexcept { subscribers.insert(id); }

void Channel::removeSubscriber(ClientId id) noexcept { subscribers.erase(id); }

Channel::Channel() : id(Channel::idGen++) {}

Channel::Channel(std::string name)
    : name(std::move(name)), id(Channel::idGen++) {}

Channel::~Channel() = default;
