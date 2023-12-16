#pragma once
#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include <arpa/inet.h>
#include <string>

class SocketAddress {
  private:
    std::string ip;
    // unsigned int port;

  public:
    SocketAddress() = default;
    explicit SocketAddress(sockaddr_in);
    SocketAddress(std::string ip, uint16_t port);

    [[nodiscard]] auto inner() const -> sockaddr_in;

    [[nodiscard]] auto fmt() -> std::string;

    [[nodiscard]] auto getIp() const noexcept -> std::string;
    [[nodiscard]] auto getPort() const noexcept -> uint16_t;

    void setPort(uint16_t);
    void setIp(std::string);

  private:
    sockaddr_in inner_addr;
};

#endif
