// Write comments explaining the purpose of each method, variable and statements
// below.

#include "server.h"

#include "logger.h"

std::mutex write_mutex;

#define SUCCESS_FLAG 1
#define IN_PROGRESS_FLAG 0

const int BUFFER_SIZE = 100;
const char READ = 'R';
const char WRITE = 'W';

int main() {
    std::vector<std::string> allData;
    // Set up the signal handler for SIGPIPE
    signal(SIGPIPE, handle_sigpipe);
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (-1 == sockfd) {
        // Output an error message if socket creation failed
        std::cerr << "Socket creation failed" << std::endl;
    }

    // Set the socket to non-blocking mode
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;

    server_addr.sin_port = htons(8080);

    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
        -1) {
        std::cerr << "Socket binding failed." << std::endl;
        return 1;
    }
    if (listen(sockfd, 5) == -1) {
        std::cerr << "Socket listening failed." << std::endl;
        return 1;
    }

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    LOG("server has started");

    try {
        while (true) {
            int new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr,
                                    &client_addr_len);
            if (new_sockfd == -1) {
                // Handle non-blocking case: no connection available
                // (EAGAIN/EWOULDBLOCK)
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    // Sleep to prevent CPU hog
                    std::this_thread::sleep_for(std::chrono::milliseconds(200));
                    continue;
                } else {
                    LOG("Error accepting connection: ");
                }
            } else {
                std::thread thread(handleConnection, new_sockfd,
                                   std::ref(allData));
                thread.detach();
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
}

void handle_sigpipe(int signum) {
    // Handle the SIGPIPE signal
    std::cerr << "SIGPIPE received, ignoring..." << std::endl;
}

int handleConnection(int client_sockfd, std::vector<std::string> &allData) {
    char buffer[BUFFER_SIZE];
    try {
        if (client_sockfd == -1) {
            std::cerr << "Socket accepting failed." << std::endl;
            return -1;
        }
        while (true) {
            ssize_t bytesRead = read(client_sockfd, buffer, sizeof(buffer) - 1);
            if (-1 == bytesRead) {
                std::cerr << "Error reading from socket." << std::endl;
                close(client_sockfd);
                return -1;
            }

            // Null-terminate the buffer
            buffer[bytesRead] = '\0';
            LOG("message was: " + STR(buffer));

            std::string bufStr(buffer);

            int retFlag = IN_PROGRESS_FLAG;
            switch (buffer[0]) {
                case WRITE:
                    handleWrite(client_sockfd, allData, bufStr, retFlag);
                    if (retFlag == SUCCESS_FLAG)
                        continue;
                    else
                        break;
                case READ:
                    handleRead(buffer, allData, client_sockfd, retFlag);
                    if (retFlag == IN_PROGRESS_FLAG) break;
                    if (retFlag == SUCCESS_FLAG) continue;
                default:
                    std::string response = "Invalid Request\n";
                    send(client_sockfd, response.c_str(), response.size(), 0);
                    close(client_sockfd);
                    return 1;
            }
        }
        std::cout << "Client did not successfully close" << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    }
    return -1;
}

void handleWrite(int client_sockfd, std::vector<std::string> &allData,
                 std::string &bufStr, int &retFlag) {
    write_mutex.lock();
    sleeptimer(3);
    allData.push_back(bufStr);
    LOG("Size of alldata: " << allData.size());
    write_mutex.unlock();
    retFlag = SUCCESS_FLAG;
    return;
}

void handleRead(char buffer[100], std::vector<std::string> &allData,
                int client_sockfd, int &retFlag) {
    // limiting reads to the size of 1 char (int4)
    int idx = buffer[1] - '0';
    if (buffer[1] == '\n' || buffer[1] == '\0' || buffer[1] == '\r') {
        idx = 0;
    } else if (idx >= allData.size()) {
        send(client_sockfd, NULL, 0, 0);
        retFlag = SUCCESS_FLAG;
        return;
    }

    DEBUG("idx < allData.size " << (idx < allData.size()) << "\n\tidx: " << idx
                                << " allData.size: " << allData.size());

    for (idx; idx < allData.size(); idx++) {
        // TODO catch error
        LOG("sending: " << allData[idx] << ", flag is :"
                        << (1 + idx == allData.size() ? 0 : MSG_MORE));

        send(client_sockfd, allData[idx].c_str(), allData[idx].size(),
             1 + idx == allData.size() ? 0 : MSG_MORE);
    }

    retFlag = SUCCESS_FLAG;
    return;
}

void sleeptimer(int sleep_count_half_secs) {
    char sleep_buffer[3];
    for (char i = 0; i < sleep_count_half_secs; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        sleep_buffer[0] = i + '0';

        if (i != sleep_count_half_secs - 1) {
            sleep_buffer[1] = '\0';
        } else {
            sleep_buffer[1] = '\n';
            sleep_buffer[2] = '\0';
        }
        std::cout << sleep_buffer << std::endl;
    }
}
