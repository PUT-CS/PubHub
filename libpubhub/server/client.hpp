#ifndef CLIENT_H
#define CLIENT_H
#include "../net/ClientSocket.hpp"

class Client {
  public:
    ClientSocket socket;
    std::wstring nickname;
    Client(ClientSocket);
    void subscribe();
    void unsubscribe();
    FileDescriptor getFd();
    void killConnection();
    
    std::string fmt();

    ~Client();
};

inline bool operator<(const Client &lhs, const Client &rhs) {
    return lhs.nickname < rhs.nickname;
}

#endif
