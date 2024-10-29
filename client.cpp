#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <thread>

int main() {
    int offset = 0;
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddy;

    serverAddy.sin_family = AF_INET;
    serverAddy.sin_port = htons(8080);
    serverAddy.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr*)&serverAddy, sizeof(serverAddy));

    std::string initConnection = "R";
    send(clientSocket, initConnection.c_str(), initConnection.size(), 0);

    while (1) {
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
        char buffer[100];
        auto bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
        buffer[bytesRead] = '\0';  // Null-terminate the buffer
        std::cout << "message was: " << buffer << std::endl;
        if (0 == bytesRead) {
            break;
        }
        offset++;
    }
    // wait for input from the user

    // send input

    // continuously update the chat log
    // print output of the server

    // save current chat log offset to allow for updating from an offset
    close(clientSocket);
}
