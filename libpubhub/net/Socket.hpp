#ifndef SOCKET_H
#define SOCKET_H

#include "SocketAddress.hpp"
#include "../common.hpp"
#include "../server/message.hpp"

class Socket {
  protected:
    SocketAddress addr;
    size_t buffer_size;
    bool created = false;

  public:
    FileDescriptor fd;
    
    void create();
    std::string receive();
    void send(Message msg);
    auto address() noexcept -> SocketAddress*;
    void shutdown();
    void close();
    
    ~Socket(){};
};

#endif