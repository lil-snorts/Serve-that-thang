#include "client.h"

#include "debug.h"

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

    connect(clientSocket, (struct sockaddr *)&serverAddy, sizeof(serverAddy));

    // TODO get a thread to do this always
    DEBUG("starting reader thread")
    std::thread readerThread(readFromServer, clientSocket);
    readerThread.detach();
    DEBUG("starting writer thread")
    while (true) {
        // wait for input from the user
        std::string userInput;
        std::getline(std::cin, userInput);

        DEBUG("user inp: " << userInput);

        userInput = "W" + userInput;
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

        // TODO this is assuming that the number is 0 <= num <= 9

        readRequest = readRequest + std::string(1, '0' + offset);
        // readRequest = readRequest + std::string(1, '0');
        send(clientSocket, readRequest.c_str(), readRequest.size(), 0);
        DEBUG("requesting data from server, Sent: " << readRequest);

        ssize_t bytesRead;
        DEBUG("data recieved:\n")

        switch (readFromSocket(bytesRead, clientSocket, buffer, offset)) {
            case -1:
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));
                break;
            default:
                std::this_thread::sleep_for(std::chrono::milliseconds(2000));
                break;
        };
    }
}

int readFromSocket(ssize_t &bytesRead, int clientSocket, char buffer[100],
                   int &offset) {
    // TODO investigate if there is a race condition, where the socket keeps
    // recieving data; will the string ever be printed or will it continiously
    // be appended to?
    std::string messageInSocket;
    DEBUG(sizeof(&buffer) + " <buffer size");
    while ((bytesRead = read(clientSocket, buffer, sizeof(&buffer) - 1)) >= 0) {
        DEBUG("bytes read>" + bytesRead)
        if (-1 == bytesRead) {
            // error encountered
            DEBUG("err")
            return -1;
        } else if (0 == bytesRead) {
            DEBUG("encountered EOF")
            std::cout << messageInSocket << std::endl;
            return 0;
        } else {
            // Null-terminate the buffer
            buffer[bytesRead] = '\0';
            DEBUG(buffer)

            for (int idx = 0; idx < 100; idx++) {
                if ('\n' == buffer[idx]) {
                    offset++;
                }
            }
            messageInSocket = messageInSocket + std::string(buffer);
        }
    };

    DEBUG("Unknown outcome")

    std::cout << messageInSocket << std::endl;
    return -1;
}
