#include "channel.hpp"
#include "types.hpp"
#include <optional>

void Channel::addSubscriber(ClientId id) noexcept { subscribers.insert(id); }

void Channel::removeSubscriber(ClientId id) noexcept { subscribers.erase(id); }

Channel::Channel() {}

Channel::~Channel() {}
