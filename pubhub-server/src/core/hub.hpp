#ifndef HUB
#define HUB

#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "queue.hpp"
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "sys/poll.h"

class Hub {
  private:
    typedef std::shared_ptr<Queue> QueuePtr;
    typedef std::vector<QueuePtr> ClientQueues;
    std::vector<std::pair<Client, ClientQueues>> clients;
    std::vector<Queue> queues;
    SocketAddress addr;
    ServerSocket *socket;

    std::vector<pollfd> poll_fds;

  public:
    Hub(SocketAddress);
    void run();
    void listen();
    void accept();

    void addClient(Client);
    void removeClient(int fd);

    void debugLogClients();
        
    ~Hub();
};

#endif
