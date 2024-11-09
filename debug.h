#define DEBUG_ENABLED false
#define DEBUG(output)                                  \
    if (DEBUG_ENABLED) {                               \
        std::cout << "DEBUG: " << output << std::endl; \
    }
