#include "SocketAddress.hpp"
#include <functional>

SocketAddress::SocketAddress(std::string ip, short unsigned port) {
    this->ip = ip;
    this->port = htons(port);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    this->inner_addr = addr;
}

SocketAddress::SocketAddress(sockaddr_in addr) {
    this->inner_addr = addr;
    this->ip = std::string(inet_ntoa(addr.sin_addr));
    this->port = addr.sin_port;
}

auto SocketAddress::inner() -> sockaddr_in& {
    return std::ref(this->inner_addr);
}

std::string SocketAddress::fmt() {
    return this->ip + ":" + std::to_string(this->port);
}
