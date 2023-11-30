#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <string>

// Made to fix include recursion :(
typedef int FileDescriptor;
typedef FileDescriptor ClientId;

typedef unsigned long ChannelId;
typedef std::string ChannelName;

#endif
