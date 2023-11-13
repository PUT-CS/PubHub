#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(SocketAddress addr) {
    this->addr = addr;
    int enableReuseAddr = 1;
    this->create();
    if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr,
                   sizeof(int)) == -1) {
        perror("setsockopt(SO_REUSEADDR) failed");
        this->close();
        throw strerror(errno);
    }
}

auto ServerSocket::bind() -> Result<None> {
    int res = ::bind(this->fd, (sockaddr *)&(*this->addr.inner()),
                     sizeof(this->addr.inner()));
    if (res == -1) {
        perror("Bind");
        return Err<None>(PubHubError::Other);
    }
    return Ok(None{});
}

auto ServerSocket::listen() -> Result<None> {
    int res = ::listen(this->fd, SOMAXCONN);
    if (res == -1) {
        perror("Listen");
        return Err<None>(PubHubError::Other);
    }
    
    return Ok(None{});
}

auto ServerSocket::accept() -> Result<ClientSocket> {
    sockaddr_in new_addr;
    socklen_t addrlen = sizeof(new_addr);
    
    int client_fd = ::accept(this->fd, (sockaddr*)&new_addr, &addrlen);
    if (client_fd == -1) {
        perror("Accept failed");
    }

    auto client_addr = SocketAddress(new_addr);
    
    return Ok(ClientSocket(client_fd, client_addr));
}

