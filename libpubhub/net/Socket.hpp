#ifndef SOCKET_H
#define SOCKET_H

#include "SocketAddress.hpp"
#include "../common.hpp"
#include "../server/types.hpp"
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
    void send(Payload &msg);
    auto address() noexcept -> const SocketAddress&;
    void kill();
    void shutdown();
    void close();
    
    ~Socket(){};
};

#endif
