#ifndef CLIENT_H
#define CLIENT_H

#include <pthread.h>
#include "chat_message.h"

typedef struct {
    int socket_fd;
    pthread_t thread_id;
    char username[32];
    MessageQueue* queue;
} Client;

int get_username_from_fd(int client_fd, char* out, size_t out_size);
Client* register_client(int client_fd, MessageQueue* queue);
int send_message_to_queue(Client* client, const char* message, MessageType type);
void* client_worker(void* arg);


#endif 