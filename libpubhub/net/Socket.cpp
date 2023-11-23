#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "../server/message.hpp"
#include "exceptions.hpp"

void Socket::create() {
    auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        throw NetworkException("Socket");
    }
    this->fd = fd;
    this->created = true;
};

std::string Socket::receive() {
    unsigned int msg_size;
    int bytes_read = 0;
    bytes_read = recv(this->fd, &msg_size, sizeof(msg_size), MSG_WAITALL);
    if (bytes_read != sizeof(msg_size)) {
	throw NetworkException("Read");
    }
    
    char message_buffer[msg_size];
    int message_bytes_read = 0;
    message_bytes_read = recv(this->fd, message_buffer, sizeof(message_buffer), MSG_WAITALL);
    if (bytes_read == -1) {
	throw NetworkException("Read");
    }
    
    return std::string(message_buffer);
};

void Socket::send(const std::string &message) {
    unsigned int msg_size = message.size();
    int bytes_send = 0;
    bytes_send = ::send(this->fd, &msg_size, sizeof(msg_size), 0);
    if (bytes_send == -1) {
	throw NetworkException("Read");
    }
    
    unsigned int message_bytes_send = 0;
    message_bytes_send = ::send(this->fd, &message, msg_size, 0);
    if (message_bytes_send == -1) {
	throw NetworkException("Read");
    }
};

auto Socket::address() noexcept -> SocketAddress * { return &this->addr; }

void Socket::kill() {
    int s = ::shutdown(this->fd, SHUT_RDWR);
    int c = ::close(this->fd);
    if (!s) {
        throw NetworkException("Shutdown or Close");
    }
}

void Socket::shutdown() {
    int res = ::shutdown(this->fd, SHUT_RDWR);
    if (res == -1) {
        throw NetworkException("Shutdown");
    }
}
void Socket::close() {
    int res = ::close(this->fd);
    if (res == -1) {
        throw NetworkException("Close");
    }
}
