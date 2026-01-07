#include "input_loop.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <pthread.h>

void* receive_thread(void *arg) {
    ClientContext *ctx = (ClientContext *)arg;
    char buffer[512];

    while (ctx->running) {
        memset(buffer, 0, sizeof(buffer));
        int n = recv(ctx->socket_fd, buffer, sizeof(buffer) - 1, 0);

        if (n < 0) {
            if (ctx->running) {
                ui_add_message(ctx->ui_ctx, "[ERROR] Failed to receive message");
            }
            break;
        } else if (n == 0) {
            ui_add_message(ctx->ui_ctx, "[INFO] Server disconnected");
            ctx->running = 0;
            break;
        } else {
            buffer[n] = '\0';

            // Format received message
            char formatted_msg[600];
            snprintf(formatted_msg, sizeof(formatted_msg), "< %s", buffer);
            ui_add_message(ctx->ui_ctx, formatted_msg);
        }
    }

    return NULL;
}

void run_client_loop(ClientContext *ctx) {
    if (!ctx || !ctx->ui_ctx) return;

    char input_buffer[256];

    while (ctx->running) {
        // Get user input
        int len = ui_get_input(ctx->ui_ctx, input_buffer, sizeof(input_buffer));

        if (len > 0) {
            // Check for quit commands
            if (strcmp(input_buffer, "/quit") == 0 || strcmp(input_buffer, "/exit") == 0) {
                ui_add_message(ctx->ui_ctx, "[INFO] Disconnecting...");
                ctx->running = 0;
                break;
            }

            // Display user's own message
            char formatted_msg[300];
            snprintf(formatted_msg, sizeof(formatted_msg), "> %s", input_buffer);
            ui_add_message(ctx->ui_ctx, formatted_msg);

            // Send to server
            int result = send(ctx->socket_fd, input_buffer, strlen(input_buffer), 0);
            if (result < 0) {
                ui_add_message(ctx->ui_ctx, "[ERROR] Failed to send message");
                ctx->running = 0;
                break;
            }
        }
    }
}
