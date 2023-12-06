#pragma once
#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include "client.hpp"

// Base class for internal errors
class InternalErrorException : public std::exception {
  private:
    std::string msg;

  public:
    InternalErrorException(const std::string &what) { this->msg = what; }
    const char *what() const noexcept override { return msg.c_str(); }
};

// Base class for user-related errors
class InvalidInputException : public std::exception {
  private:
    std::string msg;

  public:
    InvalidInputException(const std::string &what) { this->msg = what; }
    const char *what() const noexcept override { return msg.c_str(); }
};

class ChannelNotFoundException : public InvalidInputException {
    using InvalidInputException::InvalidInputException;
};
class ChannelAlreadyExistsException : public InvalidInputException {
    using InvalidInputException::InvalidInputException;
};
class ClientNotFoundException : public InvalidInputException {
    using InvalidInputException::InvalidInputException;
};
class ClientDisconnectedException : public InternalErrorException {
    using InternalErrorException::InternalErrorException;
};

#endif
