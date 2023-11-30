#pragma once
#ifndef MESSAGE_H
#define MESSAGE_H

#include "../json.hpp"

enum PayloadKind {
    Error,
    Subscribe,
    Unsubscribe,
    CreateChannel,
    DeleteChannel,
    Publish
};

enum HubError {
    NoSuchChannel,
    ChannelAlreadyExists,
    NotSubscribed,
    AlreadySubscribed,
    InternalError,
};

class Payload {
    /*
Structure: 
size | json
    */  
protected:
    // Number of characters that hold the message size, i.e. 0087 is 4
    // static const size_t SIZE_SPACE = 4;
    unsigned int size;
    nlohmann::json content;

public:
    std::string toString() {
        std::string s = content.dump();
        return s;
    }
    nlohmann::json getContent() const {
        return this->content;
    }
    // static std::string padSize(unsigned short int size) {
    //     std::stringstream s;
    //     s << std::setfill('0') << std::setw(Payload::SIZE_SPACE) << size;
    //     (void)std::setfill(' ');
    //     return s.str();
    // }
};

template <PayloadKind K>
class UtilityPayload : public Payload {
public:
    UtilityPayload(const std::string_view &target) {
	static_assert(K != Publish && K != Error, "Invalid UtilityPayload kind. It cannot be `Publish` or `Error`");
	this->content = {
            {"kind", K},
	    {"target", target}
	};
    }

    // static UtilityPayload fromString(std::string text) {
    // 	UtilityPayload<K> uPl;
    // 	uPl.size = binaryStringToNumber<unsigned short>(text.substr(0, 16));
	
    // }

    std::string getTarget() {
        return this->content["target"];
    }
};

class PublishPayload : public Payload {
public:
    PublishPayload(std::string channel, time_t valid_for, nlohmann::json content) {
	this->content = {
            {"kind", PayloadKind::Publish},
            {"channel", channel},
	    {"expiration", std::time(0) + valid_for},
            // Nested JSON
	    {"content", content}
	};
    }
    PublishPayload(std::string_view channel, time_t valid_for, time_t starting_at, nlohmann::json content) {
	this->content = {
            {"kind", PayloadKind::Publish},
            {"channel", channel},
	    {"expiration", starting_at + valid_for},
            // Nested JSON
	    {"content", content}
	};
    }

    // nlohmann::json getContent() {
    //     return this->content;
    // }

    std::string getChannel() {
        return this->content["channel"];
    }

    time_t getExpiration() {
        return this->content["expiration"];
    }

    // static PublishPayload fromString(std::string) {
	
    // }
};

class ErrorPayload : public Payload {
    public:
    /*
      Powinno zawierać JSON typu
      {
       "request" : "Subscribe [TARGET]"
       "error" : "AlreadySubscribed"
      }
     */
    ErrorPayload(std::string_view in_response_to, const HubError &what) {
        this->content = {
	    {"request", in_response_to},
	    {"error", what}
	};
        //...
    }

};

#endif