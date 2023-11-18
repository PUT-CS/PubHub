#ifndef QUEUE
#define QUEUE

#include "client.hpp"
#include "message.hpp"
#include <deque>
#include <vector>

class Queue {
    private:
    std::deque<Payload> messages;
    std::vector<Client> subscribers;
    
    public:
    Queue();
    // void send_message();    
    ~Queue(){};
};

#endif
