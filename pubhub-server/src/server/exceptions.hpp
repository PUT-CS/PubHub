#pragma once
#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include <utility>

#include "Client.hpp"

// Base class for internal errors
class InternalErrorException : public std::exception {
  private:
    std::string msg;

  public:
    explicit InternalErrorException(std::string what) : msg(std::move(what)) {}
    [[nodiscard]] auto what() const noexcept -> const char * override {
        return msg.c_str();
    }
};

// Base class for user-related errors
class InvalidInputException : public std::exception {
  private:
    std::string msg;

  public:
    explicit InvalidInputException(std::string what) : msg(std::move(what)) {}
    [[nodiscard]] auto what() const noexcept -> const char * override {
        return msg.c_str();
    }
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
