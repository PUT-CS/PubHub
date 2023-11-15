#ifndef MESSAGE_H
#define MESSAGE_H
#include "../common.hpp"
#include <string>
#include "../../include/json.hpp"

enum MessageKind {
    SUBSCRIBE,
    UNSUBSCRIBE,
    CREATE_CHANNEL,
    DELETE_CHANNEL,
    PUBLISH,
    MESSAGE_ERROR
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

   Different kinds of messages have content of different meaning:
   SUBSCRIBE, UNSUBSCRIBE - name of the channel
   CREATE_CHANNEL, DELETE_CHANNEL - name of the channel,
   PUBLISH - name of the channel and post content separated by a semicolon
 **/
class Message {
private:
    std::time_t expiration;
    unsigned short size;
    MessageKind kind;
    std::string content;
public:
    Message(unsigned short size, MessageKind kind, nlohmann::json content);
    ~Message();
};

#endif
