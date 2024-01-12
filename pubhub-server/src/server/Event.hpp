#pragma once
#ifndef EVENT_H
#define EVENT_H

#include "types.hpp"

enum class EventKind { Input, Disconnect, ConnectionRequest, Nil };

/**
   Returned from Hub::nextEvent.
   Describes an event in the event loop
   and indicates what happened and for which client
**/
struct Event {
    EventKind kind;
    FileDescriptor fd;
};

#endif
