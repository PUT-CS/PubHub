#pragma once
#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include "client.hpp"

// Base class for internal errors
class InternalErrorException : std::exception {
  private:
    std::string msg;

  public:
    InternalErrorException(const std::string &what) { this->msg = what; }
    const char *what() const noexcept override { return msg.c_str(); }
};

// Base class for user-related errors
class InvalidInputException : std::exception {
  private:
    std::string msg;

  public:
    InvalidInputException(const std::string &what) { this->msg = what; }
    const char *what() const noexcept override { return msg.c_str(); }
};

class ChannelNotFoundException : public InvalidInputException {
    using InvalidInputException::InvalidInputException;
};
class ChannelAlreadyCreatedException : InvalidInputException {
    using InvalidInputException::InvalidInputException;
};

class ClientNotFoundException : InvalidInputException {
    using InvalidInputException::InvalidInputException;
};

class ClientDisconnectedException : InternalErrorException {
    using InternalErrorException::InternalErrorException;
};

#endif
