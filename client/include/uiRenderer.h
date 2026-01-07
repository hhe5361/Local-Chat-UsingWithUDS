#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <ncurses.h>

#define MAX_MESSAGES 100
#define MAX_MESSAGE_LEN 512

typedef struct {
    char messages[MAX_MESSAGES][MAX_MESSAGE_LEN];
    int message_count;
    int scroll_offset;

    WINDOW *message_win;
    WINDOW *input_win;
    WINDOW *border_win;

    int running;
} UIContext;

// Initialize UI
int ui_init(UIContext *ctx);

// Cleanup UI
void ui_cleanup(UIContext *ctx);

// Add message to display
void ui_add_message(UIContext *ctx, const char *message);

// Refresh the display
void ui_refresh(UIContext *ctx);

// Get input from user (non-blocking)
int ui_get_input(UIContext *ctx, char *buffer, int max_len);

// Scroll up/down
void ui_scroll_up(UIContext *ctx);
void ui_scroll_down(UIContext *ctx);

#endif
