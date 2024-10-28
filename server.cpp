// Write comments explaining the purpose of each method, variable and statements
// below. Point out any bugs afterwards

#include "server.h"

int main() {
    std::vector<std::string> allData;

    // Create a socket using the socket() function
    // AF_INET: Address family for IPv4
    // SOCK_STREAM: Socket type for TCP (stream-oriented)
    // 0: Protocol (0 means the default protocol for the socket type)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket was created successfully
    if (sockfd == -1) {
        // Output an error message if socket creation failed
        std::cerr << "Socket creation failed" << std::endl;
    }
    // Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    // Define a sockaddr_in structure to specify the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;  // Set the address family to IPv4
    server_addr.sin_addr.s_addr =
        INADDR_ANY;  // Accept connections from any IP address
    server_addr.sin_port = htons(
        8080);  // Set the port number to 8080 (convert to network byte order)

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        std::cerr << "Socket binding failed." << std::endl;
        return 1;
    }
    if (listen(sockfd, 5) == -1) {
        std::cerr << "Socket listening failed." << std::endl;
        return 1;
    }

    std::vector<std::thread> client_serverThreads;

    // TODO create a list of sockaddr_in to itr through for round robbin
    // serving?
    // TODO cont. Or Highest-Priority serving. Who ever has the mutex gets
    // served till its free?

    // Define a sockaddr_in structure to specify the client address
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    // TODO while true
    while (1) {
        // TODO accept
        int new_sockfd =
            accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_sockfd == -1) {
            // Handle non-blocking case: no connection available
            // (EAGAIN/EWOULDBLOCK)
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No incoming connection, continue with other tasks
                // (non-blocking)
                continue;
            } else {
                // Other error occurred
                std::cerr << "Error accepting connection: " << errno
                          << std::endl;
            }
        } else {
            client_serverThreads.push_back(
                std::thread(handleConnection, new_sockfd, std::ref(allData)));
        }
    }
}

int handleConnection(int client_sockfd, std::vector<std::string> &allData) {
    // TODO do something with this retFlag
    bool retFlag = true;
    // TODO start thread to handle request
    if (client_sockfd == -1) {
        // Output an error message if socket accepting failed
        std::cerr << "Socket accepting failed." << std::endl;
        return 1;  // Exit the program with an error code
    }

    while (true) {
        char buffer[100];
        auto bytesRead = read(
            client_sockfd, buffer,
            sizeof(buffer) - 1);  // Read data from the client into the buffer

        // TODO wait for sema to be free

        if (bytesRead < 0) {
            std::cerr << "Error reading from socket." << std::endl;
            close(client_sockfd);
            break;
        }
        buffer[bytesRead] = '\0';  // Null-terminate the buffer
        std::cout << "message was: " << buffer << std::endl;

        // the +1 moves the poiner of the buffer to the next index
        std::string bufStr(buffer + sizeof(char));

        for (char i = 0; i < 10; i++) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (i != 9) {
                char sleep_buffer[2];
                sleep_buffer[0] = i + '0';
                sleep_buffer[1] = '\0';
                send(client_sockfd, sleep_buffer, sizeof(sleep_buffer), 0);
            } else {
                char sleep_buffer[3];
                sleep_buffer[0] = i + '0';
                sleep_buffer[1] = '\n';
                sleep_buffer[2] = '\0';
                send(client_sockfd, sleep_buffer, sizeof(sleep_buffer), 0);
            }
        }

        switch (buffer[0]) {
            case 'W':  // write
                allData.push_back(bufStr);
                // No break so the client can get the most up to date info
                break;
            case 'R':  // read
            {
                // limiting reads to the size of 1 char (int4)
                int idx = buffer[1];
                std::cout << "Index requested: " << idx << std::endl;
                if (idx == '\n' || idx == '\0' || idx == '\r') {
                    // treat like its a request for all logs
                    idx = 0;
                } else if (idx >= allData.size()) {
                    std::cout << "Invalid index" << std::endl;
                    std::string response = "Invalid Request\n";

                    send(client_sockfd, response.c_str(), response.size(), 0);
                    // TODO exit thread
                    continue;
                }

                std::cout << "idx < allData.size()" << (idx < allData.size())
                          << std::endl;
                std::cout << "idx: " << idx
                          << " allData.size: " << allData.size() << std::endl;

                for (idx; idx < allData.size(); idx++) {
                    // TODO catch error
                    std::cout << "sending: " << allData[idx] << "flag is :"
                              << (1 + idx == allData.size() ? 0 : MSG_MORE)
                              << "\n";

                    send(client_sockfd, allData[idx].c_str(),
                         allData[idx].size(),
                         1 + idx == allData.size() ? 0 : MSG_MORE);
                }
                std::cout << "Finished :)" << std::endl;
                break;
            }

            default:
                std::string response = "Invalid Request\n";

                send(client_sockfd, response.c_str(), response.size(), 0);
                close(client_sockfd);
                // TODO exit thread
        }

        std::string response = "\n";

        if (!send(client_sockfd, response.c_str(), response.size(), 0)) {
            std::cout << "Client did not successfully close" << std::endl;
        } else {
            // Output a success message if the socket was accepted successfully
            std::cout << "Client accepted successfully." << std::endl;
        }
    }
    retFlag = false;
    return retFlag;
}
