# Local-Chat-UsingWithUDS

A local chat application using Unix Domain Sockets (UDS) for inter-process communication.

## Table of Contents

- [What is Unix Domain Socket (UDS)?](#what-is-unix-domain-socket-uds)
- [Key Features of UDS](#key-features-of-uds)
- [UDS vs TCP/IP Sockets](#uds-vs-tcpip-sockets)
- [Project Structure](#project-structure)
- [Usage Example](#usage-example)

---

## What is Unix Domain Socket (UDS)?

**Unix Domain Socket (UDS)**, also known as IPC socket (Inter-Process Communication socket), is a data communications endpoint for exchanging data between processes executing on the same host operating system.

Unlike TCP/IP sockets that use the network stack, UDS uses the file system as its address namespace. Communication occurs entirely within the operating system kernel, making it faster and more efficient for local inter-process communication.

<img width="384" height="131" alt="image" src="https://github.com/user-attachments/assets/1aa467af-f483-4e87-a6a6-26566032f4d2" />


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


---

## Additional Resources

- [Unix Domain Socket - Wikipedia](https://en.wikipedia.org/wiki/Unix_domain_socket)
- [Linux man page - unix(7)](https://man7.org/linux/man-pages/man7/unix.7.html)
- [Beej's Guide to Unix IPC](https://beej.us/guide/bgipc/)

---

## License

This project is open source and available under the MIT License.

https://pubs.opengroup.org/onlinepubs/009696699/basedefs/sys/un.h.html