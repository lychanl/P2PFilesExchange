#ifndef P2PFILESEXCHANGE_LOGGER_H
#define P2PFILESEXCHANGE_LOGGER_H

#include <iostream>
#include <mutex>
#include <chrono>

class Logger
{
public:
    Logger()
    {

    };

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    std::mutex loggerMutex;

    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;

    void logMessage(std::string message);

    void logError(std::string error);

    void logDebug(std::string debug);

    void setShouldShowDebug(bool shouldShowDebug);

private:
    bool shouldShowDebug = true;

};

#endif //P2PFILESEXCHANGE_LOGGER_H
