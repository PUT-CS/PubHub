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

    const char *what() const noexcept override { return msg.c_str(); }
};
