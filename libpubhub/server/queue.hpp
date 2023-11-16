#ifndef QUEUE
#define QUEUE

#include "client.hpp"
#include <deque>
#include <vector>

class Queue {
    private:
    std::deque<Message> messages;
    std::vector<Client> subscribers;
    
    public:
    Queue();
    // void send_message();    
    ~Queue(){};
};

#endif
