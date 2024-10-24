// Write comments explaining the purpose of each method, variable and statements below. Point out any bugs afterwards

#include <iostream>     // Include the iostream library for input and output
#include <sys/socket.h> // Include the socket library for socket functions
#include <netinet/in.h> // Include the library for internet address family
#include <arpa/inet.h>  // Include the library for address conversion functions
#include <unistd.h>     // Include the library for close function
#include <vector>
int main()
{
    std::vector<std::string> allData;

    // Create a socket using the socket() function
    // AF_INET: Address family for IPv4
    // SOCK_STREAM: Socket type for TCP (stream-oriented)
    // 0: Protocol (0 means the default protocol for the socket type)
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // Check if the socket was created successfully
    if (sockfd == -1)
    {
        // Output an error message if socket creation failed
        std::cerr << "Socket creation failed" << std::endl;
    }

    // Define a sockaddr_in structure to specify the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;         // Set the address family to IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    server_addr.sin_port = htons(8080);       // Set the port number to 8080 (convert to network byte order)

    // Bind the socket to the specified address and port using the bind() function
    // sockfd: The socket file descriptor
    // (struct sockaddr *)&server_addr: Pointer to the server address structure
    // sizeof(server_addr): Size of the server address structure
    if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        // Output an error message if socket binding failed
        std::cerr << "Socket binding failed." << std::endl;
        return 1; // Exit the program with an error code
    }

    // Listen for incoming connections on the socket using the listen() function
    // sockfd: The socket file descriptor
    // 5: Maximum number of pending connections in the queue
    if (listen(sockfd, 5) == -1)
    {
        // Output an error message if socket listening failed
        std::cerr << "Socket listening failed." << std::endl;
        return 1; // Exit the program with an error code
    }

    // Define a sockaddr_in structure to specify the client address
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr); // Length of the client address structure

    while (true)
    {
        // Accept an incoming connection using the accept() function
        // sockfd: The socket file descriptor
        // (struct sockaddr *)&client_addr: Pointer to the client address structure
        // &client_addr_len: Pointer to the length of the client address structure
        int new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
        if (new_sockfd == -1)
        {
            // Output an error message if socket accepting failed
            std::cerr << "Socket accepting failed." << std::endl;
            return 1; // Exit the program with an error code
        }

        char buffer[100];
        auto bytesRead = read(new_sockfd, buffer, sizeof(buffer) - 1); // Read data from the client into the buffer

        if (bytesRead < 0)
        {
            std::cerr << "Error reading from socket." << std::endl;
            close(new_sockfd);
            continue; // Skip to the next iteration of the loop
        }
        buffer[bytesRead] = '\0'; // Null-terminate the buffer
        std::cout << "message was: " << buffer << std::endl;

        std::string bufStr(buffer);

        switch (buffer[0])
        {
        case 'R':
            /* code */
            {
                int idx = (buffer[1] - '0') * 100 + (buffer[2] - '0') * 10 + (buffer[3] - '0');
                std::cout << "Index requested: " << idx << std::endl;
                if (idx >= allData.size())
                {
                    std::cout << "Invalid index" << std::endl;
                    std::string response = "Invalid Request\n";

                    send(new_sockfd, response.c_str(), response.size(), 0);
                    // TODO does this free resources?
                    close(new_sockfd);
                    continue;
                }

                while (0, idx < allData.size(), idx++)
                {
                    // TODO catch error
                    std::cout << "sending: " << allData[idx] << std::endl;

                    // TODO how do i send many responses to the client?
                    send(new_sockfd, allData[idx].c_str(), allData[idx].size(), 1 + idx == allData.size() ? 0 : MSG_MORE);
                }
                std::cout << "Finished :)" << std::endl;
            }
            break;
        case 'W':
            allData.push_back(bufStr);
            // code for WRITE
            break;
        default:
            std::string response = "Invalid Request\n";

            send(new_sockfd, response.c_str(), response.size(), 0);
            close(new_sockfd);
            continue;
        }

        std::string response = "EOL\n";

        if (!send(new_sockfd, response.c_str(), response.size(), 0))
        {
            std::cout << "Client did not successfully close" << std::endl;
        }
        else
        {
            // Output a success message if the socket was accepted successfully
            std::cout << "Client accepted successfully." << std::endl;
        }
        close(new_sockfd);
    }
    close(sockfd);
    return 0; // Exit the program successfully
}
