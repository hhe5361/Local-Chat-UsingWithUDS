#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "client.h"
#include "publisher.h"
#define SOCKET_PATH "/tmp/local_chat_socket"
#define QUEUE_SIZE 128

int main(){
    int listen_fd;
    struct sockaddr_un addr;

    //unix domain socket creation
    listen_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    if(listen_fd < 0){
        perror("socket error");
        return -1;
    }

    //socket define 
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH); //socket listening through this file.
    unlink(SOCKET_PATH); //remove any previous socket file

    //이제 bind
    if(bind(listen_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind error");
        close(listen_fd);
        return -1;
    }

    printf("Socket creation is finished.\n");

    //listen
    if(listen(listen_fd, 5) < 0){
        perror("listen error");
        close(listen_fd);
        return -1;
    }
    printf("Server listening... %s\n", SOCKET_PATH);

    //accept loop
    Publisher publisher;
    publisher_init(&publisher);

    while(1){
        int client_fd = accept(listen_fd, NULL, NULL);
            if(client_fd < 0){
                perror("accept error");
                continue;
            }

            //client 등록
            Client* new_client = register_client(client_fd, publisher.message_queue);
            if (!new_client) {
                close(client_fd);
                continue;
            }
            add_client_to_publisher(&publisher, new_client);

        printf("New client connected: FD %d\n", client_fd);
    }

}
