#include "Socket.hpp"
#include "SocketAddress.hpp"
#include <cerrno>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <memory>
#include <netinet/in.h>
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

/**
   throws:
   - Networkexception if recv() fails
 **/
std::string Socket::receive() {
    uint32_t msg_size;
    int bytes_read = 0;
    bytes_read = recv(this->fd, &msg_size, sizeof(msg_size), MSG_WAITALL);

    // using `ntohl` here breaks the code.
        
    if (bytes_read != sizeof(msg_size) && bytes_read != 0) {
	throw NetworkException("Read");
    }
    
    char* message_buffer = new char[msg_size+1];
    int message_bytes_read = 0;
    
    message_bytes_read = recv(this->fd, message_buffer, msg_size, MSG_WAITALL);
    
    message_buffer[msg_size] = '\0'; // ?

    if (message_bytes_read != (long) msg_size && message_bytes_read != 0) {
	throw NetworkException("Read");
    }
    
    if (bytes_read == 0 || message_bytes_read == 0) {
	// TODO: Add adequate exception
	throw NetworkException("Socket already closed", false);
    }
    
    auto str = std::string(message_buffer);
    
    delete[] message_buffer;
    return str;
};

/**
   throws:
   - Networkexception if send() fails
 **/
void Socket::send(const std::string &message) {
    uint32_t msg_size = message.size();
    msg_size = htonl(msg_size);
    int bytes_send = 0;
    bytes_send = ::send(this->fd, &msg_size, sizeof(msg_size), 0);
    if (bytes_send == -1) {
	throw NetworkException("Send");
    }
    
    int message_bytes_send = 0;
    message_bytes_send = ::send(this->fd, &message, msg_size, 0);
    if (message_bytes_send == -1) {
	throw NetworkException("Send");
    }
};

auto Socket::address() noexcept -> const SocketAddress& { return this->addr; }

void Socket::kill() {
    int s = ::shutdown(this->fd, SHUT_RDWR);
    int c = ::close(this->fd);
    if (!s || !c) {
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
