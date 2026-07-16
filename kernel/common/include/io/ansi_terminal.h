/* ANSI emulation layer for terminal interface */

#ifndef __IO_ANSI_TERMINAL_H
#define __IO_ANSI_TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>

#include <io/terminal.h>

/* terminal function callbacks */
typedef enum {
    ANSI_TERMINAL_COLOUR_BLACK = 0,
    ANSI_TERMINAL_COLOUR_RED,
    ANSI_TERMINAL_COLOUR_GREEN,
    ANSI_TERMINAL_COLOUR_YELLOW,
    ANSI_TERMINAL_COLOUR_BLUE,
    ANSI_TERMINAL_COLOUR_MAGENTA,
    ANSI_TERMINAL_COLOUR_CYAN,
    ANSI_TERMINAL_COLOUR_WHITE
} ansi_terminal_colour8_t;
typedef enum {
    ANSI_TERMINAL_COLOUR_NORMAL,
    ANSI_TERMINAL_COLOUR_BRIGHT // aixterm bright/high intensity colour
} ansi_terminal_colour8_modifier_t;
typedef struct {
    void* user;

    /* basic primitives - see terminal.h */
    char (*read)(void* user); 
    void (*write)(void* user, char c);
    bool (*read_available)(void* user);
    bool (*write_available)(void* user);

    /* optional handlers for ANSI terminal control sequences */
    void (*move_cursor)(void* user, int dx, int dy); // positive = right/down, negaitve = left/up. INT_MAX = move to end (right/bottom edge), INT_MIN = move to start (left/top edge)
    void (*set_cursor)(void* user, int x, int y); // x/y set to -1 to stay at current column/row
    void (*clear_line)(void* user); // clear everything in current
    void (*clear_line_left)(void* user); // clear from beginning of line to cursor
    void (*clear_line_right)(void* user); // clear from cursor to end of line
    void (*clear_lines_before)(void* user); // clear all lines before current line
    void (*clear_lines_after)(void* user); // clear all lines after current line
    void (*clear_screen)(void* user);
    void (*reset_foreground)(void* user); // reset to default foreground
    void (*reset_background)(void* user); // reset to default background
    void (*set_foreground_8)(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier);
    void (*set_background_8)(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier);
    void (*set_foreground_256)(void* user, uint8_t colour);
    void (*set_background_256)(void* user, uint8_t colour);
    void (*set_foreground_rgb)(void* user, uint32_t colour);
    void (*set_background_rgb)(void* user, uint32_t colour);

    /* internal state tracking - set these to 0/false initially */
    enum {
        ANSI_TERMINAL_STATE_IDLE,
        ANSI_TERMINAL_STATE_ESC,
        ANSI_TERMINAL_STATE_BRACKET
    } state;
    char param_buf[32];
    size_t param_idx;
} ansi_terminal_t;

char ansi_terminal_read(void* user);
void ansi_terminal_write(void* user, char c);
bool ansi_terminal_read_available(void* user);
bool ansi_terminal_write_available(void* user);

#define ANSI_TERMINAL_INTERFACE(terminal) (terminal_t){\
    .user = (void*)(terminal), \
    .read = ansi_terminal_read, \
    .write = ansi_terminal_write, \
    .read_available = ansi_terminal_read_available, \
    .write_available = ansi_terminal_write_available \
}

#endif /* __IO_ANSI_TERMINAL_H */