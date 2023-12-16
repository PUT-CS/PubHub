#pragma once
#ifndef COMMON
#define COMMON

#include <cstdio>
#include <sstream>
#include <string>
#include <thread>
#include <ctime>
#include <iostream>
#include <exception>

#define __LOG_LEVEL__ LogLevel::DEBUG
#define __LOG_HEADER__                                                         \
    "[" + std::string(__FILE_NAME__) + ":" + std::to_string(__LINE__) +        \
        " in " + std::to_string(thread_id()) + +"]: "

#define DEBUG(msg) __LOG_LEVEL__ <= LogLevel::DEBUG? __logDebug(__LOG_HEADER__ + msg) : (void)0
#define INFO(msg) __LOG_LEVEL__ <= LogLevel::INFO? __logInfo(__LOG_HEADER__ + msg) : (void)0
#define WARN(msg) __LOG_LEVEL__ <= LogLevel::WARN? __logWarn(__LOG_HEADER__ + msg) : (void)0
#define ERROR(msg) __LOG_LEVEL__ <= LogLevel::ERROR? __logError(__LOG_HEADER__ + msg) : (void)0

// Works well for a small number of threads, debug only and should not be trusted as accurate
inline int thread_id() {
    return (unsigned)std::hash<std::thread::id>()(std::this_thread::get_id()) % 1000;
}

template <typename T> void print(T obj) { std::cout << obj << std::endl; }

enum LogLevel {DEBUG, INFO, WARN, ERROR, NONE };

// Define logMessage at the beginning
template <typename T>
inline void __logMessage(LogLevel level, const T &message) {
    std::string levelStr;
    std::string colorCode;

    switch (level) {
    case DEBUG:
        levelStr = "DEBUG";
        colorCode = "";
        break;
    case INFO:
        levelStr = "INFO";
        colorCode = "\033[32m"; // Green
        break;
    case WARN:
        levelStr = "WARN";
        colorCode = "\033[33m"; // Yellow
        break;
    case ERROR:
        levelStr = "ERROR";
        colorCode = "\033[31m"; // Red
        break;
    case NONE:
        std::terminate();
        break;    
    }

    // Reset color after the message
    std::string resetColor = "\033[0m";

    // Get the current timestamp
    // std::time_t now = std::time(nullptr);
    // std::tm* timeInfo = std::localtime(&now);
    // char timeStr[20];
    // std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", timeInfo);

    // auto output = colorCode + "[" + std::string(timeStr) + "][" + levelStr +
    // "]: " +  message + resetColor + "\n";
    auto output = colorCode + message + resetColor + "\n";

    std::cerr << output;
}

template <typename T> void __logDebug(const T &message) {
    __logMessage(DEBUG, message);
}

template <typename T> void __logInfo(const T &message) {
    __logMessage(INFO, message);
}

template <typename T> void __logWarn(const T &message) {
    __logMessage(WARN, message);
}

template <typename T> void __logError(const T &message) {
    __logMessage(ERROR, message);
}

#endif
