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
#include "../core/message.hpp"

auto Socket::create() -> Result<None> {
    auto fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("Socket");
        // auto err = std::string("Create socket: ") + strerror(errno);
        return Err<None>(PubHubError::Other);
    }
    this->fd = fd;
    this->created = true;
    return Ok(None{});
};

auto Socket::receive() -> Result<Message> {
    char buffer[this->buffer_size];
    int bytes_read = 1;
    int bytes_overall = 0;
    while (bytes_read != 0) {
        bytes_read = read(this->fd, buffer, sizeof(buffer));
        if (bytes_read == -1) {
            perror("Read");
            exit(1);
        }
        bytes_overall += bytes_read;
    }
    print_n_from(buffer, bytes_overall);
    return Err<Message>(PubHubError::Other);
};

auto Socket::send(Message msg) -> Result<None> {
    (void)msg;
    return Ok(None{});
};

auto Socket::address() -> SocketAddress * { return &this->addr; }

void Socket::shutdown() { ::shutdown(this->fd, SHUT_RDWR); }
void Socket::close() { ::close(this->fd); }
