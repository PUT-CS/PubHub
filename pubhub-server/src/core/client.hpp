#ifndef CLIENT
#define CLIENT
#include "../net/ClientSocket.hpp"

class Client {
  public:
    ClientSocket socket;
    std::wstring nickname;
    Client(ClientSocket);
    void subscribe();
    void unsubscribe();
    FileDescriptor getFd();
    
    std::string fmt();

    ~Client();
};

inline bool operator<(const Client &lhs, const Client &rhs) {
    return lhs.nickname < rhs.nickname;
}

#endif
