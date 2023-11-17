#include "message.hpp"

template <PayloadKind K>
UtilityPayload<K> UtilityPayload<K>::fromString(std::string) {
    
}

template <PayloadKind K>
std::string UtilityPayload<K>::toString(){}


PublishPayload::PublishPayload(std::string channel, long valid_for,
                               nlohmann::json content) {}

PublishPayload PublishPayload::fromString(std::string) {
    
}

std::string PublishPayload::toString() {}


BroadcastPayload::BroadcastPayload(PublishPayload) {}

BroadcastPayload BroadcastPayload::fromString(std::string) { 
    
}

std::string BroadcastPayload::toString(){}
