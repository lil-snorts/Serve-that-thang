#include "logger.h"

void log(char* toLog) { log(std::string(toLog)); }

void log(std::string toLog) {
    time_t now;

    time(&now);

    std::cout << asctime(localtime(&now)) << "[" << toLog << "]" << std::endl;
}