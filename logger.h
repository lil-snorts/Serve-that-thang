
#include <string.h>
#include <time.h>

#include <iostream>
#include <string>

#define STR(toString) std::string(toString)

#define DEBUG_ENABLED false
#define DEBUG(output)                                  \
    if (DEBUG_ENABLED) {                               \
        std::cout << "DEBUG: " << output << std::endl; \
    }

char* getLocalTime();

#define LOG(toLog) \
    std::cout << getLocalTime() << "[" << toLog << "]" << std::endl;
