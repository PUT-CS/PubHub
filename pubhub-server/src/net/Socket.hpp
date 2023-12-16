#pragma once
#include <cstddef>
#include <deque>
#ifndef SOCKET_H
#define SOCKET_H

#include "SocketAddress.hpp"
#include "../server/types.hpp"

class Socket {
  protected:
    SocketAddress addr;
    bool created = false;

  public:
    FileDescriptor fd;
    
    void create();
    std::string receive();
    void send(std::string msg);
    auto address() const noexcept -> const SocketAddress&;
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
    std::deque<std::byte> buffer;
    
    std::string fmt() const noexcept;
    
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
