#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <functional>
#include <netinet/in.h>

SocketAddress::SocketAddress(std::string ip, short unsigned port) {
    this->ip = ip;
    // this->port = htons(port);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    this->inner_addr = addr;
}

SocketAddress::SocketAddress(sockaddr_in addr) {
    this->ip = std::string(inet_ntoa(addr.sin_addr));
    this->inner_addr = addr;
}

std::string SocketAddress::getIp() const noexcept { return this->ip; }

uint16_t SocketAddress::getPort() const noexcept {
    return ntohs(this->inner_addr.sin_port);
}

void SocketAddress::setIp(std::string ip) {
    this->ip = ip;
    inner_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

void SocketAddress::setPort(uint16_t port) {
    inner_addr.sin_port = htons(port);
}

auto SocketAddress::inner() const -> sockaddr_in { return inner_addr; }

std::string SocketAddress::fmt() {
    return this->getIp() + ":" + std::to_string(this->getPort());
}
