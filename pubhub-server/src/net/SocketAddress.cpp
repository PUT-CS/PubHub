#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cstdint>
#include <functional>
#include <netinet/in.h>

SocketAddress::SocketAddress(const std::string &ip, uint16_t port)
    : ip(ip), inner_addr() {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    this->inner_addr = addr;
}

SocketAddress::SocketAddress(sockaddr_in addr) : inner_addr(addr) {
    this->ip = std::string(inet_ntoa(addr.sin_addr));
}

auto SocketAddress::getIp() const noexcept -> const std::string & {
    return this->ip;
}

auto SocketAddress::getPort() const noexcept -> uint16_t {
    return ntohs(this->inner_addr.sin_port);
}

void SocketAddress::setIp(const std::string& ip) {
    this->ip = ip;
    inner_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

void SocketAddress::setPort(uint16_t port) {
    inner_addr.sin_port = htons(port);
}

auto SocketAddress::inner() const -> sockaddr_in { return inner_addr; }

auto SocketAddress::fmt() -> std::string {
    return this->getIp() + ":" + std::to_string(this->getPort());
}
