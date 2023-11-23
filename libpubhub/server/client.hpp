#ifndef CLIENT_H
#define CLIENT_H
#include "../net/ClientSocket.hpp"
#include "message.hpp"

class Client {
  public:
    ClientSocket socket;
    std::wstring nickname;
    Client(ClientSocket);
    void subscribe();
    void unsubscribe();
    FileDescriptor getFd();
    void killConnection() noexcept;
    nlohmann::json receiveMessage();
    void sendMessage(const Payload&);
    std::string fmt();

    ~Client();
};

inline bool operator<(const Client &lhs, const Client &rhs) {
    return lhs.nickname < rhs.nickname;
}

#endif
