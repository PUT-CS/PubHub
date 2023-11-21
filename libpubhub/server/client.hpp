#ifndef CLIENT_H
#define CLIENT_H
#include "../net/ClientSocket.hpp"
#include "message.hpp"
#include "types.hpp"
#include <vector>

class Client {
  public:
    ClientSocket socket;
    std::wstring nickname;
    std::vector<ChannelId> subscriptions;
    
    Client(ClientSocket);
    
    void subscribe();
    void unsubscribe();
    FileDescriptor getFd();
    void killConnection() noexcept;
    
    std::string fmt();

    ~Client();
};

#endif
