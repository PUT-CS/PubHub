#ifndef NET_EXCEPTIONS_H
#define NET_EXCEPTIONS_H

#include <cerrno>
#include <cstring>
#include <exception>
#include <string>

class NetworkException : public std::exception {
  private:
    std::string msg;

  public:
    NetworkException(const std::string &what) {
        this->msg = what + ": " + std::string(strerror(errno));
    }
    NetworkException(const std::string &what, bool perror) {
        this->msg = what;
    }

    const char *what() const noexcept override { return msg.c_str(); }
};

#endif
