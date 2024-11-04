#include "client.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

#define DEBUG_ENABLED false
#define DEBUG(output)                                  \
    if (DEBUG_ENABLED) {                               \
        std::cout << "DEBUG: " << output << std::endl; \
    }

const char READ = 'R';
const char WRITE = 'W';

void handleError(int errCode) {
    std::cerr << "Err code " << errCode << ", ignoring." << std::endl;
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    // fcntl(clientSocket, F_SETFL, 0);
    sockaddr_in serverAddy;

    serverAddy.sin_family = AF_INET;
    serverAddy.sin_port = htons(8080);
    serverAddy.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddy, sizeof(serverAddy));

    // TODO get a thread to do this always
    std::thread readerThread(readFromServer, clientSocket);

    while (true) {
        // wait for input from the user
        std::string userInput;
        std::getline(std::cin, userInput);

        userInput = "W" + userInput;

        std::cout << "user inp: " << userInput << std::endl;
        // send input
        send(clientSocket, userInput.c_str(), userInput.size(), 0);
    }
    close(clientSocket);
}

void readFromServer(int clientSocket) {
    int offset = 0;
    char buffer[100];
    while (true) {
        std::string readRequest(1, READ);

        // TODO this is assuming that the number is 0 <= num <= 9

        readRequest = readRequest + std::string(1, '0' + offset);

        send(clientSocket, readRequest.c_str(), readRequest.size(), 0);
        DEBUG("requesting data from server, Sent: " << readRequest);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        auto bytesRead = read(clientSocket, buffer, sizeof(buffer) - 2);

        if (-1 == bytesRead) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
            DEBUG("No data")
        } else {
            // Null-terminate the buffer
            buffer[bytesRead] = '\n';
            buffer[bytesRead + 1] = '\0';

            DEBUG("data recieved:\n" << buffer << "\n");

            std::cout << ">" << buffer << std::endl;

            for (char byte : buffer) {
                if ('\n' == byte) {
                    offset++;
                }
            }
            DEBUG("offset: " << offset);
        }
    }
}
