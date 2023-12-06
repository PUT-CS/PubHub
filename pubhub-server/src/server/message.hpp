#pragma once
#include <map>
#include <string>
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
    static const inline std::map<std::string, PayloadKind> stringMap = {
        {"Subscribe", PayloadKind::Subscribe},
        {"Unsubscribe", PayloadKind::Unsubscribe},
        {"CreateChannel", PayloadKind::CreateChannel},
        {"DeleteChannel", PayloadKind::DeleteChannel},
        {"Publish", PayloadKind::Publish}};

    std::string toString() {
        std::string s = content.dump();
        return s;
    }
    nlohmann::json getContent() const { return this->content; }
};

#endif
