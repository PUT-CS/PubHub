#include "client.hpp"
#include <exception>
#include <stdexcept>
#include <string>

enum ClientError { NoSuchClient };

class ClientException : std::exception {
  private:
    std::string msg;

  public:
    ClientException(const std::string& what) {
        this->msg = what;
    }
    //ClientException(const ClientError &what) { this->msg = "Not implemented"; }

    const char *what() const noexcept override { return msg.c_str(); }
};
