#ifndef MYRESULT
#define MYRESULT

#include "common.hpp"
#include <iostream>
#include <string>
#include <variant>
#include <vector>

template <typename T> class Result {
  public:
    Result() : _data_(), _is_error_(true) {}

    bool is_error() const { return this->_is_error_; }

    T &unwrap() {
        if (_is_error_) {
            //auto err = this->get_error().to_string();
            throw std::logic_error("Tried to get value from an error Result.");
        }
        return std::get<T>(_data_);
    }

    PubHubError &get_error() {
        if (!_is_error_) {
            throw std::logic_error(
                "Tried to get error from a non-error Result.");
        }
        return std::get<PubHubError>(_data_);
    }

    std::variant<T, PubHubError> _data_;
    bool _is_error_;
};

template <typename T> Result<T> Ok(T val) {
    auto x = Result<T>();
    x._data_ = val;
    x._is_error_ = false;
    return x;
}

template <typename T>
Result<T> Err(PubHubError err) {
    auto x = Result<T>();
    x._data_ = err;
    x._is_error_ = false;
    return x;
}

#endif
