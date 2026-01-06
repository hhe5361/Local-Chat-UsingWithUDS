#include "chat_message.h"
#include <stdio.h>
#include <pthread.h>

void init_message_queue(MessageQueue* q) {
    q->head = 0;
    q->tail = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

int enqueue_message(MessageQueue* q, const ChatMessage* message) {
    pthread_mutex_lock(&q->lock);

    int next_tail = (q->tail + 1) % QUEUE_SIZE;
    if (next_tail == q->head) {
        // Queue is full
        pthread_mutex_unlock(&q->lock);
        return -1;
    }

    q->buffer[q->tail] = *message;
    q->tail = next_tail;

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
    return 0;
}

int dequeue_message(MessageQueue* q, ChatMessage* out_message) {
    pthread_mutex_lock(&q->lock);

    //큐 비어있으면 pthread cond wait 사용해서 대기
    while (q->head == q->tail) {
        pthread_cond_wait(&q->cond, &q->lock);
    }

    *out_message = q->buffer[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;

    pthread_mutex_unlock(&q->lock);
    return 0;
}