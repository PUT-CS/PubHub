#ifndef HUB
#define HUB

#include "../net/ServerSocket.hpp"
#include "client.hpp"
#include "event.hpp"
#include "queue.hpp"
#include "sys/poll.h"
#include <bits/types/time_t.h>
#include <map>
#include <memory>
#include <optional>
#include <utility>
#include <vector>


class Hub {
  private:
    typedef std::shared_ptr<Queue> QueuePtr;
    typedef std::vector<QueuePtr> ClientQueues;
    
    std::vector<std::pair<Client, ClientQueues>> clients;
    std::vector<Queue> queues;
    SocketAddress addr;
    std::unique_ptr<ServerSocket> socket;

    /**
       Holds all pollfds needed to check for socket activity.
       The first pollfd in this vector shall always be the server pollfd.
       Every time a client connects or disconnects this has to be updated
     **/
    std::vector<pollfd> poll_fds;

  public:
    Hub(SocketAddress);
    void run();
    Event nextEvent(time_t);
    void listen();
    Client accept();
    
    void addClient(Client) noexcept;
    void removeClientByFd(FileDescriptor);
    auto clientByFd(FileDescriptor) -> std::optional<std::shared_ptr<Client>>;

    void debugLogClients();
    void debugLogPollFds();

    ~Hub();
};

#endif
