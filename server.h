#pragma once
#include <arpa/inet.h>  // Include the library for address conversion functions
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>  // Include the library for internet address family
#include <sys/socket.h>  // Include the socket library for socket functions
#include <unistd.h>      // Include the library for close function

#include <iostream>  // Include the iostream library for input and output
#include <string>
#include <thread>
#include <vector>
int handleConnection(int new_sockfd, std::vector<std::string> &allData);
