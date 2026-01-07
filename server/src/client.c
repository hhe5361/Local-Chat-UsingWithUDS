#define _GNU_SOURCE
#include "client.h"
#include "chat_message.h"
#include <sys/socket.h>
#include <pwd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

//username 획득
//-> getsocketopt() 이용? UID 획득 후에 getpwuid 를 통해 username 획득
int get_username_from_fd(int client_fd, char* out, size_t out_size) {
    struct ucred cred;
    socklen_t len = sizeof(cred);

    if (getsockopt(client_fd, SOL_SOCKET, SO_PEERCRED, &cred, &len) == -1) {
        perror("getsockopt(SO_PEERCRED)");
        return -1;
    }

    ///etc/passwd 에서 username 획득
    struct passwd* pw = getpwuid(cred.uid);
    if (!pw) {
        perror("getpwuid");
        return -1;
    }

    strncpy(out, pw->pw_name, out_size - 1);
    out[out_size - 1] = '\0';

    return 0;
}

//client 등록 및 thread 생성
Client* register_client(int client_fd, MessageQueue* queue){
    Client* new_client = malloc(sizeof(Client));

    if(!new_client){
        perror("malloc error");
        return NULL;
    }

    if (get_username_from_fd(client_fd, new_client->username, sizeof(new_client->username)) < 0) {
        strncpy(new_client->username, "unknown", sizeof(new_client->username));
    }


    new_client->socket_fd = client_fd;
    new_client->queue = queue;

    pthread_create(&new_client->thread_id, NULL, client_worker, new_client);
    pthread_detach(new_client->thread_id);

    return new_client;

}

//message messagequeue에 넣기
int send_message_to_queue(Client* client, const char* message, MessageType type) {
    //message 정의
    ChatMessage msg;
    msg.from_fd = client->socket_fd;
    msg.type = type;

    strncpy(msg.username, client->username, sizeof(msg.username) - 1);
    msg.username[sizeof(msg.username) - 1] = '\0';

    strncpy(msg.message, message, sizeof(msg.message) - 1);
    msg.message[sizeof(msg.message) - 1] = '\0';

    enqueue_message(client->queue, &msg);

    return 0;
}

//thread worker
//client thread에서는 recv로 메세지 받고 message queue에 넣는 것만 반복해주면 됨. 
//arg로 Client 구조체 포인터 넘겨받으면 됨. 
void* client_worker(void* arg){
    Client* client = (Client*)arg;
    char buffer[256];
    ssize_t data;
    //join message 
    send_message_to_queue(client, "", MSG_JOIN);

    while(1){
        data = recv(client->socket_fd, buffer, sizeof(buffer) - 1, 0);
        if(data <= 0){
            if(data < 0) perror("recv error");
            else printf("Client disconnected --> FD %d\n", client->socket_fd); //여기서 사실 그거 필요한데 publisher 한테 알리는 거 ㅇㅇ... 음 어케 알릴까... 음..

            send_message_to_queue(client, "", MSG_CHAT); //message type 을 통하여 알리기.
            close(client->socket_fd);
            break;
        }

        buffer[data] = '\0'; //null terminate
        printf("Received from client FD %d: %s\n", client->socket_fd, buffer);

        send_message_to_queue(client, buffer, MSG_CHAT);
    }

    //종료할 때 일단 간단하게 "exit" 알리는 역할로 할까.
    send_message_to_queue(client, "", MSG_LEAVE);
    return NULL;

}
