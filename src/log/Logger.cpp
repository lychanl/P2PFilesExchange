#include "Logger.h"

void Logger::logMessage(std::string message)
{
    std::cout << "LOG: " << message << std::endl;
}

void Logger::logError(std::string error)
{
    std::cout << "\033[1;31m ERROR: " << error << "\033[0m\n" << std::endl;
}

void Logger::logDebug(std::string debug) {
    std::cout << "\033[1;34m DEBUG: " << debug << "\033[0m\n" << std::endl;
}
