#ifndef CHAT_H
#define CHAT_H

#include <pthread.h>

#define QUEUE_SIZE 128

typedef enum {
    MSG_JOIN,
    MSG_CHAT,
    MSG_LEAVE
} MessageType;

typedef struct {
    MessageType type;
    int from_fd;
    char username[32];
    char message[256];
} ChatMessage;

typedef struct {
    ChatMessage buffer[QUEUE_SIZE];
    int head;
    int tail;

    pthread_mutex_t lock;
    pthread_cond_t  cond;
} MessageQueue;

void init_message_queue(MessageQueue* queue);
int enqueue_message(MessageQueue* queue, const ChatMessage* message);
int dequeue_message(MessageQueue* queue, ChatMessage* out_message);

#endif