#include "message.hpp"

UtilityPayload::UtilityPayload(PayloadKind kind, std::string content) {
}

UtilityPayload UtilityPayload::fromString(std::string) {
    
}

std::string UtilityPayload::toString(){}


PublishPayload::PublishPayload(std::string channel, long valid_for,
                               nlohmann::json content) {}

PublishPayload PublishPayload::fromString(std::string) { 
    
}

std::string PublishPayload::toString() {}


BroadcastPayload::BroadcastPayload(PublishPayload) {}

BroadcastPayload BroadcastPayload::fromString(std::string) { 
    
}

std::string BroadcastPayload::toString(){}
