#ifndef SERVER_SOCKET
#define SERVER_SOCKET

#include "../common.hpp"
#include "../myresult.hpp"
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
