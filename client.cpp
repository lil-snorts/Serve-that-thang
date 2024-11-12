#include "client.h"

#include "logger.h"

const char READ = 'R';
const char WRITE = 'W';

void handleError(int errCode) {
    std::cerr << "Err code " << errCode << ", ignoring." << std::endl;
}

int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    fcntl(clientSocket, F_SETFL, O_NONBLOCK);
    sockaddr_in serverAddy;

    serverAddy.sin_family = AF_INET;
    serverAddy.sin_port = htons(8080);
    serverAddy.sin_addr.s_addr = INADDR_ANY;

    connect(clientSocket, (struct sockaddr *)&serverAddy, sizeof(serverAddy));

    DEBUG("starting reader thread")
    std::thread readerThread(readFromServer, clientSocket);
    readerThread.detach();

    DEBUG("starting writer thread")
    while (true) {
        // wait for input from the user
        std::string userInput;
        std::getline(std::cin, userInput);

        DEBUG("user inp: " << userInput);

        userInput = "W" + userInput + "\n";
        // send input
        send(clientSocket, userInput.c_str(), userInput.size(), 0);
    }
    close(clientSocket);
}

void readFromServer(int clientSocket) {
    int offset = 0;
    char buffer[100];

    DEBUG("reading from server")
    while (true) {
        std::string readRequest(1, READ);

        readRequest = readRequest + std::string(1, '0' + offset);
        send(clientSocket, readRequest.c_str(), readRequest.size(), 0);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        DEBUG("requesting data from server, Sent: " << readRequest);

        ssize_t bytesRead;
        DEBUG("data received:\n")

        switch (readFromSocket(bytesRead, clientSocket, buffer, offset)) {
            case -1:
                std::this_thread::sleep_for(std::chrono::milliseconds(4000));
                break;
            default:
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                break;
        };
    }
}

int readFromSocket(ssize_t &bytesRead, int clientSocket, char buffer[100],
                   int &offset) {
    // TODO investigate if there is a race condition, where the socket keeps
    // receiving data; will the string ever be printed or will it continuously
    // be appended to?
    std::string messageInSocket;
    while ((bytesRead = read(clientSocket, buffer, sizeof(&buffer) - 1)) >= 0) {
        if (-1 == bytesRead) {
            DEBUG("err")
            return -1;
        } else if (0 == bytesRead) {
            DEBUG("encountered EOF")
            if (messageInSocket.size() > 0) {
                LOG(messageInSocket);
            }
            return 0;
        } else {
            buffer[bytesRead] = '\0';
            DEBUG("buffer output: " << buffer);

            for (int idx = 0; idx < 100; idx++) {
                if ('\n' == buffer[idx]) {
                    offset++;
                    DEBUG("Offset++")
                } else if ('\0' == buffer[idx]) {
                    offset++;
                    DEBUG("Null terminator in the received buffer")
                    break;
                }
            }
            messageInSocket = messageInSocket + std::string(buffer);
        }
    };

    DEBUG("Unknown outcome")
    if (messageInSocket.size() > 0) {
        std::cout << messageInSocket << std::endl;
    }
    return -1;
}
