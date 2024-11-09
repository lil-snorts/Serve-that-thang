#include "logger.h"

char* getLocalTime() {
    time_t now;

    time(&now);
    char* localDateTime = asctime(localtime(&now));
    localDateTime[strlen(localDateTime) - 1] = '\0';
    return localDateTime;
}