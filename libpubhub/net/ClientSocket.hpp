#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H

#include "Socket.hpp"
#include "../server/types.hpp"
#include "SocketAddress.hpp"
#include "../common.hpp"

class ClientSocket : public Socket {
  public:
    ClientSocket();
    ClientSocket(SocketAddress);
    ClientSocket(FileDescriptor, SocketAddress);
    void connect();

    std::string fmt() noexcept;
    
    ~ClientSocket(){};
};

#endif
