#ifndef MESSAGE_H
#define MESSAGE_H
#include "../common.hpp"
#include <ctime>
#include <string>
#include "../json.hpp"
#include <bitset>

enum PayloadKind {
    Subscribe,
    Unsubscribe,
    CreateChannel,
    DeleteChannel,
    Publish,
    Error
};

enum HubError {
    NoSuchChannel,
    ChannelAlreadyExists,
    NotSubscribed,
    AlreadySubscribed,
};

class Payload {
protected:
    public:
    virtual std::string toString() = 0;
};

// nie moze byc publish ani error
// size | kind (!publish && !error) | {target: "ChannelName"}
class UtilityPayload : Payload {
    private:
    std::string content;
    PayloadKind kind;
public:
    UtilityPayload(PayloadKind, std::string);
    static UtilityPayload fromString(std::string);
    std::string toString() override;
};

// moze byc tylko publish
// size | kind (publish) | {expiration: 3, content: {}}
class PublishPayload : Payload {
private:
    std::string channel;
    nlohmann::json content;
public:
    PublishPayload(std::string channel, long valid_for, nlohmann::json content);
    static PublishPayload fromString(std::string);
    std::string toString() override;
};

// przesylany subskrybentom
// size | {expiration: 3, content: {}}
class BroadcastPayload : Payload {
private:
    std::string content;
public:
    BroadcastPayload(PublishPayload);
    static BroadcastPayload fromString(std::string);
    std::string toString() override;
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
class Message {
private:
    unsigned short size;
    PayloadKind kind;
    //std::time_t expiration;
    std::string content;
public:
    Message(PayloadKind, std::string);
    Message() {}
    std::string serialize();
    // static Message PublishTo(long valid_for, nlohmann::json content) {
    // 	auto m = Message();
    // 	m.expiration = std::time(0) + valid_for;
    // 	m.content = content.dump();
    // 	m.size = 0;
    // }
    static Message Error(HubError, std::string what);
    ~Message() {}
    
    
};

#endif
