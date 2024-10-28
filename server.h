#pragma once
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>  // Include the library for internet address family
#include <sys/socket.h>  // Include the socket library for socket functions
#include <unistd.h>      // Include the library for close function

#include <iostream>  // Include the iostream library for input and output
#include <mutex>
#include <string>
#include <thread>
#include <vector>
int handleConnection(int new_sockfd, std::vector<std::string> &allData);

void sleeptimer(int client_sockfd, int sleep_count_half_secs);
