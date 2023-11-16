#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
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
    char buffer[this->buffer_size];
    int bytes_read = 1;
    int bytes_overall = 0;
    while (bytes_read != 0) {
        bytes_read = read(this->fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            throw NetworkException("Read");
        }
        bytes_overall += bytes_read;
    }
    print_n_from(buffer, bytes_overall);
    return std::string(buffer);
};

void Socket::send(Message msg) {
    (void)msg;
    throw NetworkException("unimplemented");
};

auto Socket::address() noexcept -> SocketAddress * const { return &this->addr; }

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
