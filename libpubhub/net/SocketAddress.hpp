#pragma once
#ifndef SOCKET_ADDRESS_H
#define SOCKET_ADDRESS_H

#include <arpa/inet.h>
#include <string>

class SocketAddress {
  public:
    SocketAddress(){};
    SocketAddress(sockaddr_in);
    SocketAddress(std::string ip, short unsigned port);
    
    std::string ip;
    unsigned int port;
    
    auto inner() -> sockaddr_in&;

    std::string fmt();

  private:
    sockaddr_in inner_addr;
};

#endif
