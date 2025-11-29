# Local-Chat-UsingWithUDS

A local chat application using Unix Domain Sockets (UDS) for inter-process communication.

## Table of Contents

- [What is Unix Domain Socket (UDS)?](#what-is-unix-domain-socket-uds)
- [Key Features of UDS](#key-features-of-uds)
- [UDS vs TCP/IP Sockets](#uds-vs-tcpip-sockets)
- [Project Structure](#project-structure)
- [Implementation Guide](#implementation-guide)
  - [Server Implementation](#server-implementation)
  - [Client Implementation](#client-implementation)
- [How to Build and Run](#how-to-build-and-run)
- [Usage Example](#usage-example)

---

## What is Unix Domain Socket (UDS)?

**Unix Domain Socket (UDS)**, also known as IPC socket (Inter-Process Communication socket), is a data communications endpoint for exchanging data between processes executing on the same host operating system.

Unlike TCP/IP sockets that use the network stack, UDS uses the file system as its address namespace. Communication occurs entirely within the operating system kernel, making it faster and more efficient for local inter-process communication.

### How UDS Works

1. **Socket File**: UDS uses a special file in the file system as the address (e.g., `/tmp/chat.sock`)
2. **Kernel Space**: All data transfer happens in kernel space without network overhead
3. **Bidirectional**: Supports full-duplex communication between processes
4. **Stream or Datagram**: Supports both `SOCK_STREAM` (like TCP) and `SOCK_DGRAM` (like UDP)

```
┌─────────────────────────────────────────────────────────────┐
│                     Operating System                        │
│  ┌─────────────┐                          ┌─────────────┐   │
│  │   Server    │                          │   Client    │   │
│  │   Process   │                          │   Process   │   │
│  └──────┬──────┘                          └──────┬──────┘   │
│         │                                        │          │
│         │        ┌─────────────────────┐         │          │
│         └────────┤   Unix Domain       ├─────────┘          │
│                  │   Socket File       │                    │
│                  │   (/tmp/chat.sock)  │                    │
│                  └─────────────────────┘                    │
│                                                             │
│                     Kernel Space                            │
└─────────────────────────────────────────────────────────────┘
```

---

## Key Features of UDS

| Feature | Description |
|---------|-------------|
| **High Performance** | No network stack overhead, direct kernel memory transfer |
| **Security** | Uses file system permissions for access control |
| **Reliability** | Guaranteed delivery with `SOCK_STREAM` |
| **Low Latency** | Communication stays within the kernel |
| **File Descriptor Passing** | Can pass file descriptors between processes |
| **Credential Passing** | Can verify peer process credentials (PID, UID, GID) |

---

## UDS vs TCP/IP Sockets

| Aspect | Unix Domain Socket | TCP/IP Socket |
|--------|-------------------|---------------|
| **Scope** | Same machine only | Network-wide |
| **Performance** | Faster (no network stack) | Slower (network overhead) |
| **Address** | File system path | IP address + port |
| **Security** | File permissions | Network security |
| **Use Case** | Local IPC | Distributed systems |

---

## Project Structure

```
Local-Chat-UsingWithUDS/
├── README.md           # This documentation
├── server.c            # Chat server implementation
├── client.c            # Chat client implementation
├── Makefile            # Build automation
└── chat.sock           # Socket file (created at runtime)
```

---

## Implementation Guide

### Server Implementation

The server creates a UDS socket, binds it to a file path, and listens for client connections.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/chat.sock"
#define BUFFER_SIZE 256

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr, client_addr;
    socklen_t client_len;
    char buffer[BUFFER_SIZE];

    // 1. Create Unix Domain Socket
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Remove existing socket file
    unlink(SOCKET_PATH);

    // 3. Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    snprintf(server_addr.sun_path, sizeof(server_addr.sun_path), "%s", SOCKET_PATH);

    // 4. Bind socket to address
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 5. Listen for connections
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Chat Server started. Waiting for connections...\n");
    printf("Socket path: %s\n", SOCKET_PATH);

    // 6. Accept client connection
    client_len = sizeof(client_addr);
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd == -1) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client connected!\n");

    // 7. Chat loop
    while (1) {
        // Receive message from client
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read <= 0) {
            printf("Client disconnected.\n");
            break;
        }

        printf("Client: %s", buffer);

        // Send response
        printf("Server: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        write(client_fd, buffer, strlen(buffer));
    }

    // 8. Cleanup
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
```

### Client Implementation

The client connects to the server's UDS socket and exchanges messages.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/chat.sock"
#define BUFFER_SIZE 256

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create Unix Domain Socket
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Configure server address
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    snprintf(server_addr.sun_path, sizeof(server_addr.sun_path), "%s", SOCKET_PATH);

    // 3. Connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to Chat Server!\n");
    printf("Type your messages (Ctrl+C to quit):\n");

    // 4. Chat loop
    while (1) {
        // Send message to server
        printf("You: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        write(client_fd, buffer, strlen(buffer));

        // Receive response from server
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read <= 0) {
            printf("Server disconnected.\n");
            break;
        }

        printf("Server: %s", buffer);
    }

    // 5. Cleanup
    close(client_fd);

    return 0;
}
```

---

## How to Build and Run

### Prerequisites

- GCC compiler
- Linux/Unix operating system

### Manual Compilation

```bash
# Compile server
gcc -o server server.c

# Compile client
gcc -o client client.c
```

### Using Makefile

Create a `Makefile`:

```makefile
CC = gcc
CFLAGS = -Wall -Wextra

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm -f server client /tmp/chat.sock
```

Then run:

```bash
make        # Build both server and client
make clean  # Remove binaries and socket file
```

### Running the Application

1. **Start the server** (in one terminal):
   ```bash
   ./server
   ```

2. **Start the client** (in another terminal):
   ```bash
   ./client
   ```

3. **Start chatting!**

---

## Usage Example

### Terminal 1 (Server)
```
$ ./server
Chat Server started. Waiting for connections...
Socket path: /tmp/chat.sock
Client connected!
Client: Hello, Server!
Server: Hi, Client! How are you?
Client: I'm good, thanks!
Server: Great to hear!
```

### Terminal 2 (Client)
```
$ ./client
Connected to Chat Server!
Type your messages (Ctrl+C to quit):
You: Hello, Server!
Server: Hi, Client! How are you?
You: I'm good, thanks!
Server: Great to hear!
```

---

## Additional Resources

- [Unix Domain Socket - Wikipedia](https://en.wikipedia.org/wiki/Unix_domain_socket)
- [Linux man page - unix(7)](https://man7.org/linux/man-pages/man7/unix.7.html)
- [Beej's Guide to Unix IPC](https://beej.us/guide/bgipc/)

---

## License

This project is open source and available under the MIT License.
