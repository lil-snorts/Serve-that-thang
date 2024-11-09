
#include <time.h>

#include <iostream>
#include <string>

#define DEBUG_ENABLED false
#define DEBUG(output)                                  \
    if (DEBUG_ENABLED) {                               \
        std::cout << "DEBUG: " << output << std::endl; \
    }

void log(const std::string toLog);
void log(char* toLog);