#pragma once
#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"

class ServerSocket : public Socket {
  public:
    ServerSocket(SocketAddress);
    void bind();
    void listen();
    ClientSocket accept();
    ~ServerSocket(){};
};

#endif
