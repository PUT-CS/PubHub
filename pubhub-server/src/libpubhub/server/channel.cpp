#include "channel.hpp"

void Channel::addSubscriber(ClientId id) noexcept { subscribers.insert(id); }

void Channel::removeSubscriber(ClientId id) noexcept { subscribers.erase(id); }

Channel::Channel() {}

Channel::~Channel() {}
