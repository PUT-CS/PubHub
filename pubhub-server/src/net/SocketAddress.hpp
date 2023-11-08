#ifndef SOCKET_ADDRESS
#define SOCKET_ADDRESS

#include <arpa/inet.h>
#include <memory>
#include <netinet/in.h>
#include <string>

class SocketAddress {
  public:
    std::string ip;
    unsigned int port;
    SocketAddress(){};
    SocketAddress(sockaddr_in);
    SocketAddress(std::string ip, short unsigned port);
    auto inner() -> std::shared_ptr<sockaddr_in>;

  private:
    sockaddr_in _inner;
};

#endif
