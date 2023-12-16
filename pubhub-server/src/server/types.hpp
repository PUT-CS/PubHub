#pragma once
#include <cstdint>
#ifndef TYPES_H
#define TYPES_H

#include <string>

// Made to fix include recursion :(
using FileDescriptor = int;
using ClientId = FileDescriptor;

using ChannelId = uint64_t;
using ChannelName = std::string;

#endif
