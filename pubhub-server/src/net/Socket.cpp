#include "Socket.hpp"
#include "../common.hpp"
#include "exceptions.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/**
   Throws:
   - **NetworkException** if socket() fails or socket already closed
 **/
void Socket::create() {
    auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw NetworkException("Socket");
    }
    this->fd = fd;
};

/**
   Throws:
   - **NetworkException** if recv() fails or socket already closed
 **/
auto Socket::receive() -> std::string {
    uint32_t msg_size = 0;
    ssize_t bytes_read =
        recv(this->fd, &msg_size, sizeof(msg_size), MSG_WAITALL);

    msg_size = ntohl(msg_size);

    if (bytes_read != sizeof(msg_size) && bytes_read != 0) {
        throw NetworkException("Read");
    }

    // auto message_buffer = std::make_unique<char[]>(msg_size + 1);
    std::string message_buffer(msg_size, 0);

    ssize_t message_bytes_read =
        recv(this->fd, message_buffer.data(), msg_size, MSG_WAITALL);

    message_buffer[msg_size] = '\0'; // check if this can be deleted

    if (message_bytes_read != static_cast<int64_t>(msg_size) &&
        message_bytes_read != 0) {
        throw NetworkException("Read");
    }

    if (bytes_read == 0 || message_bytes_read == 0) {
        throw NetworkException("Socket already closed", false);
    }

    return message_buffer;
};

/**
   Throws:
   - **NetworkException** if send() fails
 **/
void Socket::send(std::string message) {
    uint32_t net_msg_size = htonl(message.size());
    if (::send(this->fd, &net_msg_size, sizeof(net_msg_size), 0) == -1) {
        throw NetworkException("Send");
    }

    if (::send(this->fd, message.data(), message.size(), 0) == -1) {
        throw NetworkException("Send");
    }
}

auto Socket::address() const noexcept -> const SocketAddress & {
    return this->addr;
}

auto Socket::getFd() noexcept -> FileDescriptor { return this->fd; }

/**
   Throws:
   - **NetworkException** if shutdown() or close() fails
 **/
void Socket::kill() {
    int s = ::shutdown(this->fd, SHUT_WR);
    int c = ::close(this->fd);
    if (!s || !c) {
        throw NetworkException("Shutdown or Close");
    }
}
/**
   Throws:
   - **NetworkException** if shutdown() fails
 **/
void Socket::shutdown() {
    int res = ::shutdown(this->fd, SHUT_RDWR);
    if (res == -1) {
        throw NetworkException("Shutdown");
    }
}

/**
   Throws:
   - **NetworkException** if close() fails
 **/
void Socket::close() {
    int res = ::close(this->fd);
    if (res == -1) {
        throw NetworkException("Close");
    }
}

//
// ServerSocket methods
//

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

/**
   Throws:
   - **NetworkException** if bind() fails
 **/
void ServerSocket::bind() {
    auto addr = this->address().inner();
    int res =
        ::bind(this->fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (res == -1) {
        throw NetworkException("Bind");
    }
}

/**
   Throws:
   - **NetworkException** if close() fails
 **/
void ServerSocket::listen() {
    int res = ::listen(this->fd, SOMAXCONN);
    if (res == -1) {
        throw NetworkException("Listen");
    }
}

/**
   Throws:
   - **NetworkException** if accept() fails
 **/
auto ServerSocket::accept() -> ClientSocket {
    sockaddr_in new_addr{};
    socklen_t addrlen = sizeof(new_addr);

    int client_fd =
        ::accept(this->fd, reinterpret_cast<sockaddr *>(&new_addr), &addrlen);
    if (client_fd == -1) {
        throw NetworkException("Accept");
    }

    auto client_addr = SocketAddress(new_addr);

    return {client_fd, client_addr};
}

//
// ClientSocket methods
//
ClientSocket::ClientSocket(SocketAddress addr) {
    this->addr = addr;
    this->create();
}

ClientSocket::ClientSocket() = default;

ClientSocket::ClientSocket(FileDescriptor fd, SocketAddress addr) {
    this->addr = addr;
    this->fd = fd;
}

auto ClientSocket::fmt() const noexcept -> std::string {
    return "CLIENT SOCKET:\nFD: " + std::to_string(this->fd) +
           " ADDRESS: " + this->address().getIp() + ":" +
           std::to_string(this->address().getPort());
}

/**
   Throws:
   - **NetworkException** if connect() fails
**/
void ClientSocket::connect() {
    auto addr = this->address().inner();
    int res =
        ::connect(this->fd, reinterpret_cast<sockaddr *>(&addr), sizeof(addr));
    if (res == -1) {
        throw NetworkException("Connect");
    }
}
