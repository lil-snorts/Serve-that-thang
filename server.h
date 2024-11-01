#pragma once
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>  // Include the library for internet address family
#include <signal.h>
#include <sys/socket.h>  // Include the socket library for socket functions
#include <unistd.h>      // Include the library for close function

#include <cstring>   // For strerror
#include <iostream>  // Include the iostream library for input and output
#include <mutex>
#include <string>
#include <thread>
#include <vector>
int handleConnection(int new_sockfd, std::vector<std::string> &allData);

void handleWriteOrErrorOutClient(int client_sockfd,
                                 std::vector<std::string> &allData,
                                 std::string &bufStr, int &retFlag);

void handleRead(char buffer[100], std::vector<std::string> &allData,
                int client_sockfd, int &retFlag);

void sleeptimer(int client_sockfd, int sleep_count_half_secs);
void handle_sigpipe(int signum);
