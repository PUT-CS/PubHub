// #include "message.hpp"

// template <PayloadKind K>
// UtilityPayload<K>::UtilityPayload(std::string  s) {
//     static_assert(K != Publish && K != Error, "Invalid UtilityPayload kind");
//     this->content = {
// 	{"target", s}
//     };
// }

// template <PayloadKind K>
// UtilityPayload<K> UtilityPayload<K>::fromString(std::string content) {
// }

// template <PayloadKind K>
// std::string UtilityPayload<K>::toString(){
//     return this->content.dump();
// }


// template <PayloadKind K>
// PublishPayload<K>::PublishPayload(std::string channel, long valid_for,
//                                nlohmann::json content) {
//     static_assert(K == Publish, "Invalid Publish");
//     this->channel = channel;
    
// }

// template <PayloadKind K>
// PublishPayload<K> PublishPayload<K>::fromString(std::string) {
    
// }

// template <PayloadKind K>
// std::string PublishPayload<K>::toString() {}


// BroadcastPayload::BroadcastPayload(PublishPayload<Publish>) {}

// BroadcastPayload BroadcastPayload::fromString(std::string) { 
    
// }

// std::string BroadcastPayload::toString(){}
