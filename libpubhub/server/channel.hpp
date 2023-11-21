#ifndef QUEUE_H
#define QUEUE_H

#include "client.hpp"
#include "message.hpp"
#include "types.hpp"
#include <vector>


class Channel {
    private:
    std::vector<FileDescriptor> subscribers;
    
    public:
    Channel();
    ~Channel(){};
};

#endif
