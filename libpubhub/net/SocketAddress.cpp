#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>

SocketAddress::SocketAddress(std::string ip, short unsigned port) {
    this->ip = ip;
    this->port = htons(port);

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    this->_inner = addr;
}

SocketAddress::SocketAddress(sockaddr_in addr) {
    this->_inner = addr;
    this->ip = std::string(inet_ntoa(addr.sin_addr));
    this->port = addr.sin_port;
}

auto SocketAddress::inner() -> std::shared_ptr<sockaddr_in> {
    return std::make_shared<sockaddr_in>(this->_inner);
}

std::string SocketAddress::fmt() {
    return this->ip + ":" + std::to_string(this->port);
}
