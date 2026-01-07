#ifndef INPUT_LOOP_H
#define INPUT_LOOP_H

#include "uiRenderer.h"

typedef struct {
    int socket_fd;
    UIContext *ui_ctx;
    int running;
} ClientContext;

// Thread function to receive messages from server
void* receive_thread(void *arg);

// Main input loop
void run_client_loop(ClientContext *ctx);

#endif
