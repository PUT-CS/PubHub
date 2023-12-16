#pragma once
#include <cstddef>
#include <deque>
#ifndef SOCKET_H
#define SOCKET_H

#include "../server/types.hpp"
#include "SocketAddress.hpp"

class Socket {
  public:
    SocketAddress addr;
    FileDescriptor fd;

  public:
    [[nodiscard]] auto getFd() noexcept -> FileDescriptor;
    [[nodiscard]] auto address() const noexcept -> const SocketAddress &;
    void create();
    [[nodiscard]] auto receive() -> std::string;
    void send(std::string msg);
    void kill();
    void shutdown();
    void close();

    ~Socket() = default;
};

class ClientSocket : public Socket {
  public:
    ClientSocket();
    explicit ClientSocket(SocketAddress);
    ClientSocket(FileDescriptor, SocketAddress);
    void connect();

    [[nodiscard]] auto fmt() const noexcept -> std::string;

    ~ClientSocket() = default;
};

class ServerSocket : public Socket {
  public:
    explicit ServerSocket(SocketAddress);
    void bind();
    void listen();

    [[nodiscard]] auto accept() -> ClientSocket;

    ~ServerSocket() = default;
};

#endif
