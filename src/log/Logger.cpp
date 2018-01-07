#include "Logger.h"

void Logger::logMessage(std::string message)
{
    std::lock_guard<std::mutex> guard(loggerMutex);

    auto time = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(time);

    std::cout << "LOG: " << std::ctime(&timeT) << " " << message << std::endl;
}

void Logger::logError(std::string error)
{
    std::lock_guard<std::mutex> guard(loggerMutex);

    auto time = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(time);

    std::cout << "\033[1;31m ERROR: " << std::ctime(&timeT) << " " << error << "\033[0m\n" << std::endl;
}

void Logger::logDebug(std::string debug) {
    std::lock_guard<std::mutex> guard(loggerMutex);

    auto time = std::chrono::system_clock::now();
    std::time_t timeT = std::chrono::system_clock::to_time_t(time);

    std::cout << "\033[1;34m DEBUG: " << std::ctime(&timeT) << " " <<debug << "\033[0m\n" << std::endl;
}
