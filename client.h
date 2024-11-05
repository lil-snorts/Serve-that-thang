#pragma once
void readFromServer(int clientSocket);

void readFromSocket(ssize_t &bytesRead, int clientSocket, char buffer[100],
                    int &offset);
