#include "Socket.hpp"
#include <cstdlib>
#include <memory>
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

/**
   throws:
   - Networkexception if recv() fails
 **/
std::string Socket::receive() {
    uint32_t msg_size = 0;
    int bytes_read = 0;
    bytes_read = recv(this->fd, &msg_size, sizeof(msg_size), MSG_WAITALL);

    msg_size = ntohl(msg_size);
        
    if (bytes_read != sizeof(msg_size) && bytes_read != 0) {
	throw NetworkException("Read");
    }
    
    auto message_buffer = std::make_unique<char[]>(msg_size+1);
    int message_bytes_read = 0;
    
    message_bytes_read = recv(this->fd, message_buffer.get(), msg_size, MSG_WAITALL);
    
    message_buffer[msg_size] = '\0'; // check if this can be deleted

    if (message_bytes_read != (long) msg_size && message_bytes_read != 0) {
	throw NetworkException("Read");
    }
    
    if (bytes_read == 0 || message_bytes_read == 0) {
	throw NetworkException("Socket already closed", false);
    }
    
    auto str = std::string(message_buffer.get());
    
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

//
// ClientSocket methods
//

ClientSocket::ClientSocket(SocketAddress addr) {
    this->addr = addr;
    this->create();
}
ClientSocket::ClientSocket(){};

ClientSocket::ClientSocket(FileDescriptor fd, SocketAddress addr) {
    this->addr = addr;
    this->fd = fd;
}

std::string ClientSocket::fmt() noexcept {
    return "CLIENT SOCKET:\nFD: " + std::to_string(this->fd) +
        " ADDRESS: " + this->address().ip
        + ":" +
        std::to_string(this->address().port);
}

void ClientSocket::connect() {
    int res = ::connect(this->fd, (sockaddr *)&this->addr.inner(),
                        sizeof(this->addr.inner()));
    if (res == -1) {
        throw NetworkException("Connect");
    }
}
