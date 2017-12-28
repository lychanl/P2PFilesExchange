#ifndef P2PFILESEXCHANGE_LOGGER_H
#define P2PFILESEXCHANGE_LOGGER_H

#include <iostream>

class Logger
{
public:
    Logger(){};

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    Logger(Logger const &) = delete;

    void operator=(Logger const &) = delete;

    void logMessage(std::string message);

    void logError(std::string error);

    void logDebug(std::string debug);

};

#endif //P2PFILESEXCHANGE_LOGGER_H
