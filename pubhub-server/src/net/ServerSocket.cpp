#include "ServerSocket.hpp"
#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

ServerSocket::ServerSocket(SocketAddress addr) {
    this->addr = addr;
    this->create();
}

auto ServerSocket::bind() -> Result<None> {
    int err = ::bind(this->fd, (sockaddr *)&(*this->addr.inner()),
                     sizeof(this->addr.inner()));
    perror("Bind");
    if (err == -1) {
        return Err<None>(PubHubError::Other);
    }
    return Ok(None{});
}

auto ServerSocket::listen() -> Result<None> {
    int err = ::listen(this->fd, SOMAXCONN);
    
    perror("Listen");
    if (err == -1) {
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

