#ifndef CLIENT_SOCKET
#define CLIENT_SOCKET

#include "Socket.hpp"
#include "SocketAddress.hpp"
#include "../common.hpp"
#include "../myresult.hpp"

class ClientSocket : public Socket {
  public:
    ClientSocket(SocketAddress);
    ClientSocket();
    ClientSocket(FileDescriptor, SocketAddress);
    auto connect() -> Result<None>;
    ~ClientSocket(){};
};

#endif
