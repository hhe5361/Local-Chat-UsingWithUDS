#ifndef PUBLISHER_H
#define PUBLISHER_H

#include <pthread.h>
#include "chat_message.h"

// Forward declaration - full definition in client.h
typedef struct Client Client;

//와 c 레전드
typedef struct {
    Client** clients;
    int client_count;
    int max_clients;
} ClientList;

typedef struct {
    MessageQueue* message_queue;
    ClientList*   client_list; //client 목록 직접 관리.
    pthread_t     thread_id;
} Publisher;

int  publisher_init(Publisher* pub);
int add_client_to_publisher(Publisher* pub, Client* client);
int remove_client_from_publisher(Publisher* pub, Client* client);
void* publisher_worker(void* arg);
int publish_to_client(Client* client, const ChatMessage* message);

#endif