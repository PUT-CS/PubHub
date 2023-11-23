#ifndef MESSAGE_H
#define MESSAGE_H
#include "../common.hpp"
#include <bits/types/time_t.h>
#include <cstddef>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <string>
#include "../json.hpp"
#include <bitset>
#include <string_view>
#include <type_traits>

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
    /*
Structure: 
size | json
    */  
protected:
    // Number of characters that hold the message size, i.e. 0087 is 4
    static const size_t SIZE_SPACE = 4;
    
    nlohmann::json content;

public:
    std::string toString() {
        std::string s = content.dump();
        std::string size_str = Payload::padSize(s.size());
        return size_str + s;
    }
    
    static std::string padSize(unsigned short int size) {
        std::stringstream s;
        s << std::setfill('0') << std::setw(Payload::SIZE_SPACE) << size;
        (void)std::setfill(' ');
        return s.str();
    }
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

    nlohmann::json getContent() {
        return this->content;
    }

    std::string getChannel() {
        return this->content["channel"];
    }

    time_t getExpiration() {
        return this->content["expiresAt"];
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
    ErrorPayload(const Payload &in_response_to, const HubError &what) {
        (void)in_response_to;
        (void)what;
        //...
    }

};

#endif
