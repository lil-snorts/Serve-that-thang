#pragma once

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>
#include <thread>

void readFromServer(int clientSocket);

int readFromSocket(ssize_t &bytesRead, int clientSocket, char buffer[100],
                   int &offset);
