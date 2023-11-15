#ifndef SOCKET
#define SOCKET

#include "SocketAddress.hpp"
#include "../common.hpp"
#include "../myresult.hpp"
#include "../core/message.hpp"

typedef int FileDescriptor;

class Socket {
  protected:
    SocketAddress addr;
    size_t buffer_size;
    bool created = false;

  public:
    FileDescriptor fd;
    auto create() -> Result<None>;
    auto receive() -> Result<Message>;
    auto send(Message msg) -> Result<None>;
    auto address() -> SocketAddress*;
    void shutdown();
    void close();
    ~Socket(){
        //this->shutdown();
        //this->close();
    };
};

#endif
