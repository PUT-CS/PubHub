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
    nlohmann::json content;
    unsigned short size;
public:
    virtual std::string toString() = 0;
};

template <PayloadKind K>
class UtilityPayload : Payload {
public:
    /**
       size | kind (!publish && !error) | {target: ""}
    **/
    UtilityPayload(std::string  content) {
	static_assert(K != Publish && K != Error, "Invalid UtilityPayload kind");
	this->content = {
	    {"target", content}
	};
    }

    // static UtilityPayload fromString(std::string text) {
    // 	UtilityPayload<K> uPl;
    // 	uPl.size = binaryStringToNumber<unsigned short>(text.substr(0, 16));
	
    // }

    /**
     metoda ta zwraca stringa z calym payloadem jako string
     
     size jest obliczany zanim zostanie doklejony na poczatku finalnego stringa
    **/
    std::string toString() override {
	std::string payload;
	payload += std::to_string(K) + " " + content.dump();
	this->size = payload.size();
	payload = std::to_string(this->size) + " " + payload;
	return payload;
    }
};

template <PayloadKind K>
class PublishPayload : Payload {
private:
    std::string channel;
public:
    /**
       size | kind (publish) | {expiration: long, content: {}}
    **/
    PublishPayload(std::string channel, long valid_for,
		   nlohmann::json content) {
	static_assert(K == Publish, "Invalid PublishPayload kind");
	this->channel = channel;
	this->content = {
	    {"expiration", valid_for},
	    {"content", content}
	};
    }
    
    nlohmann::json get_content() {
	return this->content;
    }

    // static PublishPayload fromString(std::string) {
	
    // }

    /**
     metoda ta zwraca stringa z calym payloadem jako string
     
     size jest obliczany zanim zostanie doklejony na poczatku finalnego stringa
    **/
    std::string toString() override {
	std::string payload;
	payload += std::to_string(K) + " " + content.dump();
	this->size = payload.size();
	payload = std::to_string(this->size) + " " + payload;
	return payload;
    }
};

class BroadcastPayload : Payload {
public:
    /**
       size | {expiration: long, content: {}}
    **/
    BroadcastPayload(PublishPayload<Publish> ppayload) {
	this->content = ppayload.get_content();
	this-> size = this->content.size();
    }
    
    // static BroadcastPayload fromString(std::string) {
	
    // }
    
    /**
       metoda ta zwraca stringa z calym payloadem jako string
     
       size jest obliczany zanim zostanie doklejony na poczatku finalnego stringa
    **/
    std::string toString() override {
	std::string payload = content.dump();
	this->size = payload.size();
	payload = std::to_string(this->size) + payload;
	return payload;
    }
};

#endif
