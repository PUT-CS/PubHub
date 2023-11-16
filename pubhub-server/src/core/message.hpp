#ifndef MESSAGE_H
#define MESSAGE_H
#include "../common.hpp"
#include <ctime>
#include <string>
#include "../../include/json.hpp"
#include <bitset>

enum PayloadKind {
    SUBSCRIBE,
    UNSUBSCRIBE,
    CREATE_CHANNEL,
    DELETE_CHANNEL,
    PUBLISH,
    MESSAGE_ERROR
};

enum HubError {
    NoSuchChannel,
    ChannelAlreadyExists,
    NotSubscribed,
    AlreadySubscribed,
};

class Payload {
    private:
    unsigned short size;
    
    public:
    virtual std::string serialize() = 0;
};

//nie moze byc publish ani error
class UtilityPayload : Payload {
    UtilityPayload(PayloadKind, std::string) {

    }
};

// moze byc tylko publish
class PublishPayload : Payload {
    public:
    PublishPayload(std::string channel, long valid_for, nlohmann::json content) {
	
    }
};

/**
   Represents a message in PubHub.
   It has a kind, content and a timestamp marking its creation.

   Messages need to be received one-by-one,
   so we need to ensure that since we're using TCP.

   We do it by sending the message size as the first 4 bytes.
   This limits the size to the reasonable ceiling of ~9990 characters.

   Message template:
   SIZE(4)|KIND(5)|CONTENT(rest)
   Numbers in parenthesis indicate field size in bytes.

//    Different kinds of messages have content of different meaning:
//    SUBSCRIBE, UNSUBSCRIBE - name of the channel
//    CREATE_CHANNEL, DELETE_CHANNEL - name of the channel,
//    PUBLISH - name of the channel and post content separated by a semicolon
//  **/
// class Message {
// private:
//     unsigned short size;
//     MessageKind kind;
//     //std::time_t expiration;
//     std::string content;
// public:
//     Message(MessageKind, std::string);
//     std::string serialize();
//     // static Message PublishTo(long valid_for, nlohmann::json content) {
//     // 	auto m = Message();
//     // 	m.expiration = std::time(0) + valid_for;
//     // 	m.content = content.dump();
//     // 	m.size = 0;
//     // }
//     static Message Error(HubError, std::string what);
    
    
//     ~Message();
// };

#endif
