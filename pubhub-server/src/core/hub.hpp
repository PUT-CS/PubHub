#ifndef HUB
#define HUB

#include "client.hpp"
#include "queue.hpp"
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "../net/ServerSocket.hpp"

class Hub {
    private:
    typedef std::shared_ptr<Queue> QueuePtr;
    typedef std::vector<QueuePtr> ClientQueues;
    std::vector<std::pair<Client, ClientQueues>> clients;
    std::vector<Queue> queues;
    SocketAddress addr;
    ServerSocket *socket;
    
    public:
    Hub(SocketAddress);
    void run();
    void listen();
    void accept();
    ~Hub();
};

#endif
