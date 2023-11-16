#include "message.hpp"

std::string Message::serialize() {
    std::string serialized_message;
    std::bitset<sizeof(unsigned short) * 8> bitsize(this->size);
    serialized_message += bitsize.to_string();
    std::bitset<sizeof(int) * 8> bitkind(this->size);
    serialized_message += bitkind.to_string();
    serialized_message += this->content;
    
    return serialized_message;
}

Message::Message(PayloadKind kind, std::string content) {
    this->kind = kind;
    std::cout << "Input message content: ";
    std::cin >> this->content;
}

Message::~Message() {}
