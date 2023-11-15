#ifndef SOCKET
#define SOCKET

#include "SocketAddress.hpp"
#include "../common.hpp"
#include "../myresult.hpp"

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
    auto address() noexcept -> SocketAddress* const;
    void shutdown();
    void close();
    
    ~Socket(){};
};

#endif
