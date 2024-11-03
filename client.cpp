#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

const char READ = 'R';
const char WRITE = 'W';

int main() {
    int offset = 0;
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    sockaddr_in serverAddy;

    serverAddy.sin_family = AF_INET;
    serverAddy.sin_port = htons(8080);
    serverAddy.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddy, sizeof(serverAddy));

    while (true) {
        std::string readRequest(1, READ);
        readRequest = readRequest + std::string(1, '0' + offset);

        send(clientSocket, readRequest.c_str(), readRequest.size(), 0);
        std::cout << "Sent: " << readRequest << std::endl;

        std::cout << "DEBUG: requesting initial data from server " << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        char buffer[100];
        auto bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        std::cout << "read from the server" << std::endl;
        buffer[bytesRead] = '\0';

        // Null-terminate the buffer

        std::cout << "message was:\n" << buffer << std::endl;

        if (-1 == bytesRead) {
            break;
        }

        for (char byte : buffer) {
            if ('\n' == byte) {
                offset++;
            }
        }
    }

    // while (true) {
    std::cout << "offset: " << offset << std::endl;
    // wait for input from the user

    std::string userInput;
    std::cin >> userInput;

    std::cout << "DEBUG: user inp: " << userInput;
    // send input
    send(clientSocket, userInput.c_str(), userInput.size(), 0);

    // continuously update the chat log
    // print output of the server

    // save current chat log offset to allow for updating from an offset
    // }
    close(clientSocket);
}
