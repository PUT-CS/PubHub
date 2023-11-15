#include "message.hpp"

Message::Message(unsigned short size, MessageKind kind, nlohmann::json content) {
    this->expiration = std::time(0);
    this->size = size;
    this->kind = kind;
    this->content = content;
}

Message::~Message() {}
