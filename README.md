# Simple Client-Server Communication Example

This project demonstrates a basic client-server communication setup using sockets in C++. The server listens for connections from clients, processes messages to either read or write data, and responds accordingly. This setup includes multi-threading, non-blocking sockets, and basic signal handling for a robust interaction.

---

## Project Structure

- **client.h** and **server.h**: Header files defining the client and server functionalities.
- **logger.h**: Provides logging functions for debugging and informational purposes.
- **main.cpp**: Implements the client-server communication with multi-threading and error handling.

## How It Works

1. **Server**: Listens on port 8080, accepts client connections, and processes commands. It supports:
    - Writing data to a server-stored collection (`WRITE` command).
    - Reading data based on index position (`READ` command).
  
2. **Client**: Connects to the server, reads user input, and sends commands to either write or request data. Runs with two threads:
    - A writer thread sends user-provided input.
    - A reader thread requests and outputs server-stored data periodically.

## Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/lil-snorts/Serve-that-thang.git
   ```

2. Compile and run the server:

   ```bash
   g++ server.cpp -o server; ./server
   ```

3. Compile and run the client:

   ```bash
   g++ client.cpp -o client; ./client
   ```

## Usage

The client opens up a console reader and prints out messages that are on the server and sends messages to the server from the user.
The server listens for specific commands from the client. Here’s how you can interact with it via cURL:

### CURL Requests

The server operates on port `8080` and interprets two commands: `WRITE` and `READ`.

#### 1. Write Data to Server

Send a message to store on the server:

```bash
curl -X POST http://localhost:8080 --data "WYour message here"
```

- **Example**: To write "Hello, Server!", you would run:

  ```bash
  curl -X POST http://localhost:8080 --data "WHello, Server!"
  ```

#### 2. Read Data from Server by Index

Retrieve data stored at a specific index:

```bash
curl -X POST http://localhost:8080 --data "R<index>"
```

- **Example**: To read data at index 0, use:

  ```bash
  curl -X POST http://localhost:8080 --data "R0"
  ```

---

### Example Interactions

1. **Write Messages**:

   ```bash
   curl -X POST http://localhost:8080 --data "WFirst Message"
   curl -X POST http://localhost:8080 --data "WSecond Message"
   ```

2. **Read Messages**:

   ```bash
   curl -X POST http://localhost:8080 --data "R0"
   curl -X POST http://localhost:8080 --data "R1"
   ```

   or get all messages

   ```bash
   curl -X POST http://localhost:8080 --data "R"
   ```

---

## Code Walkthrough

### Client Code

- `main`: Establishes a socket connection to the server and starts two threads:
  - **Reader thread**: Periodically requests data from the server.
  - **Writer thread**: Takes user input and sends it to the server.
- `readFromServer`: Handles reading messages from the server.

### Server Code

- `main`: Sets up a socket listener, accepts client connections, and processes requests in separate threads.
- `handleConnection`: Receives data, determines if it's a `WRITE` or `READ` command, and calls the appropriate handler.
- `handleWrite`: Adds a message to the server's internal data list.
- `handleRead`: Retrieves and sends data back to the client based on the specified index.

## TODOs

### Repository

- [ ] Complete this README with request formats and examples.
- [ ] Implement pipeline tests to automate project testing and ensure stability.

### Server

- [x] Add a `write`/POST function for clients to write to the chat log.
  - [ ] Support larger payloads.
  - [x] Enforce client turn-taking to manage writes.
- [x] Add a `read`/GET function for clients to retrieve chat logs.
- [x] Implement mutexes for concurrent access management.
  - [x] Read requests are mutex-free for faster performance.
  - [x] Write requests are mutex-protected to avoid data race conditions.
- [ ] Enable usernames for message identification.
- [x] Add timestamps for message tracking.
- [ ] Implement message log redundancy and persistence.
- [ ] **Idle Timeout**: Threads should have an idle timeout mechanism to avoid resource wastage.
- [ ] Unit tests

### Client

- [x] Develop a basic client application.
- [x] Auto-update chat log to reflect current messages.
- [x] Parse all entered data as `write` requests.
- [ ] Enable username attachment for chat messages.
- [ ] Enable READ requests for indexes greater than 9
- [ ] Unit tests

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any enhancements or bug fixes.

## Code Overview

The server code includes a `write` mutex to synchronize client access to the chat log. Each client connection is handled in a separate thread, and the server supports both read and write operations.

### Notable Bugs/Issues

- [x] When clients send requests in quick succession, the socket buffer will store multiple requests and string them together to act like the same message
  - possible fix is suffixing messages with Newlines `\n`
- [x] When clients send a message and the readThread requests for new messages they will be treated as the same message ![alt text](readMeBugSection/image.png)
  - possible fix: server reads from the buffer, parses the messages then services each message.
- [x] client re-requests the initial payload from server.
  - Possible error is the initial setting of the offset?
  - Investigation: the client is sending 2 read requests for offset 0 because it is not waiting for first response. Leading to a doubling of offset
  - Temp solution: add a timed delay after each read call, this will give the server some time to respond.
  - Ideal solution: add redundancy to each call so that if the server send a previous message it wont be duplicated for the client

---

Thank you for checking out this project! Looking forward to contributions and ideas to make it better. Happy coding!
