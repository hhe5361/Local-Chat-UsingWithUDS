#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/local_chat_socket"

int main () {
    int fd;
	struct sockaddr_un addr;

    //unix domain socket creation
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
        return -1;
    }

    //socket 설정 define
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX; //unix domain 지정해주고
    strcpy(addr.sun_path, SOCKET_PATH); //socket file attach 해주면 됨.

    //conenct 시도
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect error");
        close(fd);
        return -1;
    }

    printf("connected to server at %s\n", SOCKET_PATH);
    
    const char* msg = "Hello from client!";
    if (send(fd, msg, strlen(msg), 0) < 0) {
        perror("send error");
        close(fd);
        return -1;
    }

    char buffer[256];
    int n = recv(fd, buffer, sizeof(buffer) - 1, 0);
    if (n < 0) {
        perror("recv error");
        close(fd);
        return -1;
    }
    buffer[n] = '\0';

    printf("received from server: %s\n", buffer);

    return 0;
}