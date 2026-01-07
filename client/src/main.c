#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "uiRenderer.h"
#include "input_loop.h"

#define SOCKET_PATH "/tmp/local_chat_socket"

int main () {
    int fd;
    struct sockaddr_un addr;

    // Unix domain socket creation
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket error");
        return -1;
    }

    // Socket 설정 define
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    // Connect 시도
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect error");
        close(fd);
        return -1;
    }

    // Initialize UI
    UIContext ui_ctx;
    if (ui_init(&ui_ctx) < 0) {
        fprintf(stderr, "Failed to initialize UI\n");
        close(fd);
        return -1;
    }

    // Display welcome message
    ui_add_message(&ui_ctx, "=== Local Chat Client ===");
    ui_add_message(&ui_ctx, "[INFO] Connected to server");
    ui_add_message(&ui_ctx, "[INFO] Type your message and press Enter to send");
    ui_add_message(&ui_ctx, "[INFO] Type /quit or /exit to disconnect");
    ui_add_message(&ui_ctx, "");

    // Setup client context
    ClientContext client_ctx = {
        .socket_fd = fd,
        .ui_ctx = &ui_ctx,
        .running = 1
    };

    // Create receiver thread
    pthread_t recv_thread;
    if (pthread_create(&recv_thread, NULL, receive_thread, &client_ctx) != 0) {
        ui_add_message(&ui_ctx, "[ERROR] Failed to create receiver thread");
        ui_cleanup(&ui_ctx);
        close(fd);
        return -1;
    }

    // Run main input loop
    run_client_loop(&client_ctx);

    // Cleanup
    client_ctx.running = 0;
    pthread_join(recv_thread, NULL);

    ui_cleanup(&ui_ctx);
    close(fd);

    return 0;
}