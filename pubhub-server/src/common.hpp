#ifndef COMMON
#define COMMON

#include <charconv>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <ostream>
#include <variant>

typedef struct {
} None;

std::ostream &operator<<(std::ostream &os, const None &_);

typedef enum  {
    Other
} PubHubError;

// template <typename T, typename U>
// std::ostream &operator<<(std::ostream &os, const cpp::result<T, U> &res) {
//     res.has_value() ? os << res.value() : os << res.error();
//     return os;
// }

template <typename T> void print(T obj) { std::cout << obj << std::endl; }

template <typename T> void print_n_from(T arg[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        std::cout << arg[i];
    }
    std::cout << "|STOP|" << std::endl;
}

#include <iostream>
#include <string>
#include <ctime>

enum LogLevel { INFO, WARN, ERROR };

// Define logMessage at the beginning
template <typename T>
void logMessage(LogLevel level, const T& message) {
    std::string levelStr;
    std::string colorCode;

    switch (level) {
        case INFO:
            levelStr = "INFO";
            colorCode = "\033[32m";  // Green
            break;
        case WARN:
            levelStr = "WARN";
            colorCode = "\033[33m";  // Yellow
            break;
        case ERROR:
            levelStr = "ERROR";
            colorCode = "\033[31m";  // Red
            break;
    }

    // Reset color after the message
    std::string resetColor = "\033[0m";

    // Get the current timestamp
    std::time_t now = std::time(nullptr);
    std::tm* timeInfo = std::localtime(&now);
    char timeStr[20];
    std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

    // Log the message with the timestamp and colored log level
    std::cout<< colorCode << "[" << timeStr << "]" << "[" << levelStr << "]" << ": " << message << resetColor << std::endl;
}

template <typename T>
void logInfo(const T& message) {
    logMessage(INFO, message);
}

template <typename T>
void logWarn(const T& message) {
    logMessage(WARN, message);
}

template <typename T>
void logError(const T& message) {
    logMessage(ERROR, message);
}

#endif
