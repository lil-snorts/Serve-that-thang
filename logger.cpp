#include "logger.h"

char* getLocalTime() {
    time_t now;

    time(&now);
    char* localDateTime = asctime(localtime(&now));
    localDateTime[strlen(localDateTime) - 1] = '\0';
    return localDateTime;
}

void log(char* toLog) { log(std::string(toLog)); }

void log(std::ostream& toLog) {
    std::cout << getLocalTime() << "[";
    toLog << "]" << std::endl;
}

void log(std::string toLog) {
    std::cout << getLocalTime() << "[" << toLog << "]" << std::endl;
}
