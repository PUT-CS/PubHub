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
    public:
    virtual std::string toString() = 0;
};

// nie moze byc publish ani error
// size | kind (!publish && !error) | {target: "ChannelName"}
template <PayloadKind K>
class UtilityPayload : Payload {
private:
    nlohmann::json content;    
public:
    UtilityPayload(std::string  s) {
	static_assert(K != Publish && K != Error, "Invalid UtilityPayload kind");
	this->content = {
	    {"target", s}
	};
    }
    static UtilityPayload fromString(std::string);
    std::string toString() override {
	return this->content.dump();
    }
};

// moze byc tylko publish
// size | kind (publish) | {expiration: 3, content: {}}
template <PayloadKind K>
class PublishPayload : Payload {
private:
    std::string channel;
    nlohmann::json content;
public:
    PublishPayload(std::string channel, long valid_for,
		   nlohmann::json content) {
	static_assert(K == Publish, "Invalid Publish");
	this->channel = channel;
    }
    static PublishPayload fromString(std::string);
    std::string toString() override;
};

// przesylany subskrybentom
// size | {expiration: 3, content: {}}
class BroadcastPayload : Payload {
private:
    std::string content;
public:
    BroadcastPayload(PublishPayload<Publish>);
    static BroadcastPayload fromString(std::string);
    std::string toString() override;
};

#endif
