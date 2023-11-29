#ifndef COMMON
#define COMMON

#include <charconv>
#include <cstddef>
#include <ctime>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <string>
#include <ctime>
#include <math.h>

template <typename T> void print(T obj) { std::cout << obj << std::endl; }

template <typename T> void print_n_from(T arg[], size_t n) {
    for (size_t i = 0; i < n; i++) {
        std::cout << arg[i];
    }
    std::cout << "|STOP|" << std::endl;
}

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
    std::cerr<< colorCode << "[" << timeStr << "]" << "[" << levelStr << "]" << ": " << message << resetColor << std::endl;
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
