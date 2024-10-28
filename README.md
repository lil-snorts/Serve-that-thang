# Basic TCP Server and Client

This repository contains a simple implementation of a TCP server and client in C++. The server listens for incoming connections, accepts messages from clients, and can respond back. This project serves as a foundation for building a chat application.

## Features

- Basic TCP server that accepts client connections
- Clients can send messages to the server
- Server responds with a confirmation message

## Getting Started

### Prerequisites

- C++ compiler (e.g., g++, clang++)
- Basic knowledge of socket programming

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/lil-snorts/tcp-server-client.git
   cd tcp-server-client
   ```

2. Compile the server and client:

   ```bash
   g++ -o server server.cpp
   g++ -o client client.cpp
   ```

3. Run the server:

   ```bash
   ./server
   ```

4. In a separate terminal, run the client:

   ```bash
   ./client
   ```

## TODOs

### Repo

- [] Create comprehensive readme.md
  - [] Add request formats
- [ ] create pipeline tests

### Server

1. [x] Add a `write`/POST function for the server to allow clients to write to the chat log.
    1. [ ] add functionality to allow clients to send larger payloads
    2. [x] Add functionality to ensure clients must wait their turn before the logs are written to.
2. [x] Add a `read`/GET function for the server to allow clients to retrieve the entire chat log.
3. [x] Implement mutexes so that multiple clients can interact at the same time
   - [x] Read requests will not wait for the mutex
   - [x] Write requests will check for the mutex first
4. [ ] Implement usernames for messages to identify who is sending messages.
5. [ ] Add timestamps to messages to indicate when they were received by the server.
6. [ ] Implement redundancy for message logs and allow for persistence.

### Client

1. [ ] Create a basic client
2. [ ] Client will automatically update with the current chat log
3. [ ] Client will parse all entered data as Write requests
4. [ ] Client will add username to chat messages

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.
