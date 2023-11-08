#include "ClientSocket.hpp"
#include "Socket.hpp"
#include "SocketAddress.hpp"

ClientSocket::ClientSocket(SocketAddress addr) {
    this->addr = addr;
    this->create();
}
ClientSocket::ClientSocket(){};

ClientSocket::ClientSocket(FileDescriptor fd, SocketAddress addr) {
    this->addr = addr;
    this->fd = fd;
}

std::string ClientSocket::fmt() {
    return "CLIENT SOCKET:\nFD: " + std::to_string(this->fd) +
        " ADDRESS: " + this->address()->ip
        + ":" +
        std::to_string(this->address()->port);
}

auto ClientSocket::connect() -> Result<None> {
    int err = ::connect(this->fd, (sockaddr *)&(*this->addr.inner()),
                        sizeof(this->addr.inner()));
    if (err == -1) {
        // return cpp::fail(std::string("Connect socket: ") + strerror(errno));
        return Err<None>(PubHubError::Other);
    }
    return Ok(None{});
}
