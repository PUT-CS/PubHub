#pragma once
#ifndef SOCKET_H
#define SOCKET_H

#include "SocketAddress.hpp"
#include "../server/types.hpp"

class Socket {
  protected:
    SocketAddress addr;
    size_t buffer_size;
    bool created = false;

  public:
    FileDescriptor fd;
    
    void create();
    std::string receive();
    void send(const std::string &msg);
    auto address() noexcept -> const SocketAddress&;
    void kill();
    void shutdown();
    void close();
    
    ~Socket(){};
};


class ClientSocket : public Socket {
  public:
    ClientSocket();
    ClientSocket(SocketAddress);
    ClientSocket(FileDescriptor, SocketAddress);
    void connect();

    std::string fmt() noexcept;
    
    ~ClientSocket(){};
};


class ServerSocket : public Socket {
  public:
    ServerSocket(SocketAddress);
    void bind();
    void listen();
    ClientSocket accept();
    ~ServerSocket(){};
};


#endif
