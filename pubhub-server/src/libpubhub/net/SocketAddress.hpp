#pragma once
#include <cstdint>
#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include <arpa/inet.h>
#include <string>

class SocketAddress {
  private:
    std::string ip;
    // unsigned int port;

  public:
    SocketAddress(){};
    SocketAddress(sockaddr_in);
    SocketAddress(std::string ip, short unsigned port);

    auto inner() const -> sockaddr_in;

    std::string fmt();

    std::string getIp() const noexcept;
    uint16_t getPort() const noexcept;

    void setPort(uint16_t);
    void setIp(std::string);

  private:
    sockaddr_in inner_addr;
};

#endif
