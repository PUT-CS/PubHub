#ifndef SERVER_EXCEPTIONS_H
#define SERVER_EXCEPTIONS_H

#include "client.hpp"
#include <exception>
#include <stdexcept>
#include <string>

class ClientException : std::exception {
  private:
    std::string msg;

  public:
    ClientException(const std::string& what) {
        this->msg = what;
    }

    const char *what() const noexcept override { return msg.c_str(); }
};

#endif
