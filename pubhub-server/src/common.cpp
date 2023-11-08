#include <iostream>
#include <ostream>
#include "common.hpp"

std::ostream &operator<<(std::ostream &os, const None &_) {
    (void)_;
    os << "None";
    return os;
}
