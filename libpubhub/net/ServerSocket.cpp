#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include "exceptions.hpp"

ServerSocket::ServerSocket(SocketAddress addr) {
    this->addr = addr;
    int enableReuseAddr = 1;
    this->create();
    int res = setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr,
                         sizeof(int));
    if (res == -1) {
        perror("setsockopt");
        this->close();
        std::exit(1);
    }
}

void ServerSocket::bind() {
    int res = ::bind(this->fd, (sockaddr*)&this->addr.inner(),
                     sizeof(this->addr.inner()));
    if (res == -1) {
        throw NetworkException("Bind");
    }
}

void ServerSocket::listen() {
    int res = ::listen(this->fd, SOMAXCONN);
    if (res == -1) {
        throw NetworkException("Listen");
    }
}

ClientSocket ServerSocket::accept() {
    sockaddr_in new_addr;
    socklen_t addrlen = sizeof(new_addr);
    
    int client_fd = ::accept(this->fd, (sockaddr*)&new_addr, &addrlen);
    if (client_fd == -1) {
        throw NetworkException("Accept");
    }

    auto client_addr = SocketAddress(new_addr);
    
    return ClientSocket(client_fd, client_addr);
}

