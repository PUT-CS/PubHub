#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"
#include "exceptions.hpp"
#include <cerrno>
#include <cstring>

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
        " ADDRESS: " + this->address()->ip
        + ":" +
        std::to_string(this->address()->port);
}

void ClientSocket::connect() {
    int res = ::connect(this->fd, (sockaddr *)&(*this->addr.inner()),
                        sizeof(this->addr.inner()));
    if (res == -1) {
        throw NetworkException("Connect");
    }
}
