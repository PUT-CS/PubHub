#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "types.hpp"

// Maybe `enum class` later?
enum EventKind {
    Input,
    Disconnect,
    ConnectionRequest,
    Nil
};

/**
   Returned from Hub::nextEvent.
   Describes an event in the event loop
   and indicates what happened and for which client
**/
struct Event {
    EventKind kind;
    FileDescriptor fd;
};
// class Event {
//   public:
//     EventKind kind;
//     int clientFd;

//     Event(EventKind kind, FileDescriptor fd) : kind(kind), clientFd(fd) {}
// };

#endif
