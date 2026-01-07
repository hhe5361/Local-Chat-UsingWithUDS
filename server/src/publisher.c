#include "publisher.h"
#include "client.h"
#include "chat_message.h"
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>


int publisher_init(Publisher* publisher) {
    publisher->message_queue = malloc(sizeof(MessageQueue));
    if (!publisher->message_queue) return -1;
    init_message_queue(publisher->message_queue);

    publisher->client_list = malloc(sizeof(ClientList));
    if (!publisher->client_list) return -1;
    publisher->client_list->clients = malloc(sizeof(Client*) * 100); //100 max
    if (!publisher->client_list->clients) return -1;
    publisher->client_list->client_count = 0;
    publisher->client_list->max_clients = 100;

    //worker thread로 바로 쓰레드 돌리게
    pthread_create(&publisher->thread_id, NULL, publisher_worker, publisher);
    pthread_detach(publisher->thread_id);

    return 0;
}

int add_client_to_publisher(Publisher* pub, Client* client){
    ClientList* clist = pub->client_list;

    if(clist->client_count >= clist->max_clients){
        return -1;
    }

    clist->clients[clist->client_count++] = client;
    return 0;
}

int remove_client_from_publisher(Publisher* pub, Client* client){
    ClientList* clist = pub->client_list;
    int found_index = -1;

    for(int i = 0; i < clist->client_count; i++){
        if(clist->clients[i] == client){
            found_index = i;
            break;
        }
    }

    //에러 처리
    if(found_index == -1){
        return -1; 
    }

    //배열 shift
    for(int i = found_index; i < clist->client_count - 1; i++){
        clist->clients[i] = clist->clients[i + 1];
    }
    clist->client_count--;

    return 0;
}

//thread worker
void* publisher_worker(void* arg) {
    Publisher* publisher = (Publisher*)arg;
    MessageQueue* queue = publisher->message_queue;
    ChatMessage message;

    while (1) {
        if (dequeue_message(queue, &message) == 0) {
            //publish all clients
            ClientList* clist = publisher->client_list;
            Client* disconnected_client = NULL;

            for (int i = 0; i < clist->client_count; i++) {

                //close된 client publish 제외. 
                if ((message.type == MSG_LEAVE || message.type == MSG_JOIN) &&
                    clist->clients[i]->socket_fd == message.from_fd) {
                    if (message.type == MSG_LEAVE) {
                        disconnected_client = clist->clients[i];
                    }
                    continue;
                }
                publish_to_client(clist->clients[i], &message);
            }
            printf("publish message from %s: %s\n", message.username, message.message);

            //remove disconnected client from list
            if (disconnected_client) {
                remove_client_from_publisher(publisher, disconnected_client);
                free(disconnected_client);
            }
        }
    }
}

int publish_to_client(Client* client, const ChatMessage* message) {
    char buffer[512];
    int len;

    if(message->type == MSG_LEAVE){
        len = snprintf(buffer, sizeof(buffer), "[%s] left chat.\n", message->username);
    } else if (message->type == MSG_JOIN){
        len = snprintf(buffer, sizeof(buffer), "[%s] join chat.\n", message->username);
    }else{
        len = snprintf(buffer, sizeof(buffer), "[%s] %s\n", message->username, message->message);
    }

    if (send(client->socket_fd, buffer, len, MSG_NOSIGNAL) < 0) {
        //client likely disconnected, ignore error
        return -1;
    }

    return 0;
}
