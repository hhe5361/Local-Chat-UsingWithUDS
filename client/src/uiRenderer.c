#include "uiRenderer.h"
#include <string.h>
#include <stdlib.h>

int ui_init(UIContext *ctx) {
    if (!ctx) return -1;

    memset(ctx, 0, sizeof(UIContext));
    ctx->running = 1;

    // Initialize ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    // Hide cursor
    curs_set(1);

    // Get screen dimensions
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    // Create windows
    // Message window (top area - leaving 4 lines for input + borders)
    int msg_height = max_y - 4;
    ctx->message_win = newwin(msg_height, max_x, 0, 0);
    scrollok(ctx->message_win, TRUE);

    // Border line
    ctx->border_win = newwin(1, max_x, msg_height, 0);

    // Input window (bottom area - 3 lines)
    ctx->input_win = newwin(3, max_x, msg_height + 1, 0);
    keypad(ctx->input_win, TRUE);

    // Draw initial border
    wattron(ctx->border_win, A_REVERSE);
    for (int i = 0; i < max_x; i++) {
        mvwaddch(ctx->border_win, 0, i, ' ');
    }
    mvwprintw(ctx->border_win, 0, 2, " Type your message below ");
    wattroff(ctx->border_win, A_REVERSE);

    // Refresh all windows
    refresh();
    wrefresh(ctx->message_win);
    wrefresh(ctx->border_win);
    wrefresh(ctx->input_win);

    return 0;
}

void ui_cleanup(UIContext *ctx) {
    if (!ctx) return;

    if (ctx->message_win) delwin(ctx->message_win);
    if (ctx->border_win) delwin(ctx->border_win);
    if (ctx->input_win) delwin(ctx->input_win);

    endwin();
}

void ui_add_message(UIContext *ctx, const char *message) {
    if (!ctx || !message) return;

    // Add message to buffer
    if (ctx->message_count < MAX_MESSAGES) {
        strncpy(ctx->messages[ctx->message_count], message, MAX_MESSAGE_LEN - 1);
        ctx->messages[ctx->message_count][MAX_MESSAGE_LEN - 1] = '\0';
        ctx->message_count++;
    } else {
        // Shift messages up
        memmove(ctx->messages[0], ctx->messages[1],
                sizeof(ctx->messages[0]) * (MAX_MESSAGES - 1));
        strncpy(ctx->messages[MAX_MESSAGES - 1], message, MAX_MESSAGE_LEN - 1);
        ctx->messages[MAX_MESSAGES - 1][MAX_MESSAGE_LEN - 1] = '\0';
    }

    ui_refresh(ctx);
}

void ui_refresh(UIContext *ctx) {
    if (!ctx) return;

    // Clear and redraw message window
    werase(ctx->message_win);

    int max_y, max_x;
    getmaxyx(ctx->message_win, max_y, max_x);

    // Calculate how many messages to display
    int start_idx = 0;
    if (ctx->message_count > max_y) {
        start_idx = ctx->message_count - max_y + ctx->scroll_offset;
        if (start_idx < 0) start_idx = 0;
    }

    // Display messages
    int y = 0;
    for (int i = start_idx; i < ctx->message_count && y < max_y; i++) {
        mvwprintw(ctx->message_win, y++, 0, "%s", ctx->messages[i]);
    }

    wrefresh(ctx->message_win);
    wrefresh(ctx->border_win);
    wrefresh(ctx->input_win);
}

int ui_get_input(UIContext *ctx, char *buffer, int max_len) {
    if (!ctx || !buffer) return -1;

    werase(ctx->input_win);
    mvwprintw(ctx->input_win, 0, 0, "> ");
    wrefresh(ctx->input_win);

    // Set timeout for non-blocking input during edit
    wtimeout(ctx->input_win, -1);

    echo();
    curs_set(1);

    // Get line from input window
    int result = wgetnstr(ctx->input_win, buffer, max_len - 1);

    noecho();
    curs_set(0);

    if (result == OK) {
        buffer[max_len - 1] = '\0';
        werase(ctx->input_win);
        wrefresh(ctx->input_win);
        return strlen(buffer);
    }

    return -1;
}

void ui_scroll_up(UIContext *ctx) {
    if (!ctx) return;
    if (ctx->scroll_offset > 0) {
        ctx->scroll_offset--;
        ui_refresh(ctx);
    }
}

void ui_scroll_down(UIContext *ctx) {
    if (!ctx) return;

    int max_y, max_x;
    getmaxyx(ctx->message_win, max_y, max_x);

    int max_scroll = ctx->message_count - max_y;
    if (max_scroll < 0) max_scroll = 0;

    if (ctx->scroll_offset < max_scroll) {
        ctx->scroll_offset++;
        ui_refresh(ctx);
    }
}
