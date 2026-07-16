#include <drivers/video/vga_text.h>
#include <io/ansi_terminal.h>
#include <string.h>

void vga_text_term_write(void* user, char c) {
    (void)user;
    vga_text_putc(c);
}

terminal_t vga_text_terminal = {
    .user = NULL,

    .read = NULL,
    .write = vga_text_term_write,
    .read_available = NULL,
    .write_available = NULL
};

void vga_text_ansi_move_cursor(void* user, int dx, int dy) {
    uint8_t x, y; vga_text_get_cursor(&x, &y);
    dx += x; dy += y;
    vga_text_ansi_set_cursor(user, dx, dy);
}

void vga_text_ansi_set_cursor(void* user, int x, int y) {
    (void)user;
    if (x < 0) x = 0; else if (x >= VGA_TEXT_WIDTH) x = VGA_TEXT_WIDTH - 1;
    if (y < 0) y = 0; else if (y >= VGA_TEXT_HEIGHT) y = VGA_TEXT_HEIGHT - 1;
    vga_text_set_cursor(x, y);
}

void vga_text_ansi_clear_line(void* user) {
    (void)user;
    uint8_t y; vga_text_get_cursor(NULL, &y);
    memset((void*)&vga_text_buffer[y * VGA_TEXT_WIDTH], 0, VGA_TEXT_WIDTH * 2);
}

void vga_text_ansi_clear_line_left(void* user) {
    (void)user;
    uint8_t x, y; vga_text_get_cursor(&x, &y);
    memset((void*)&vga_text_buffer[y * VGA_TEXT_WIDTH], 0, y * 2);
}

void vga_text_ansi_clear_line_right(void* user) {
    (void)user;
    uint8_t x, y; vga_text_get_cursor(&x, &y);
    memset((void*)&vga_text_buffer[y * VGA_TEXT_WIDTH + x], 0, (VGA_TEXT_WIDTH - x) * 2);
}

void vga_text_ansi_clear_lines_before(void* user) {
    (void)user;
    uint8_t y; vga_text_get_cursor(NULL, &y);
    memset((void*)vga_text_buffer, 0, y * VGA_TEXT_WIDTH * 2);
}

void vga_text_ansi_clear_lines_after(void* user) {
    (void)user;
    uint8_t y; vga_text_get_cursor(NULL, &y);
    memset((void*)&vga_text_buffer[(y + 1) * VGA_TEXT_WIDTH], 0, (VGA_TEXT_HEIGHT - y - 1) * VGA_TEXT_WIDTH * 2);
}

void vga_text_ansi_clear_screen(void* user) {
    (void)user;
    vga_text_clear();
}

void vga_text_ansi_set_foreground(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier) {
    (void)user;
    uint8_t attrib = vga_text_get_attrib() & ~VGA_TEXT_FG_MASK;
    switch (modifier) {
        case ANSI_TERMINAL_COLOUR_BRIGHT:
            switch (colour) {
                case ANSI_TERMINAL_COLOUR_BLACK: vga_text_set_attrib(attrib | VGA_TEXT_FG_DARK_GREY); break;
                case ANSI_TERMINAL_COLOUR_RED: vga_text_set_attrib(attrib | VGA_TEXT_FG_LIGHT_RED); break;
                case ANSI_TERMINAL_COLOUR_GREEN: vga_text_set_attrib(attrib | VGA_TEXT_FG_LIGHT_GREEN); break;
                case ANSI_TERMINAL_COLOUR_YELLOW: vga_text_set_attrib(attrib | VGA_TEXT_FG_YELLOW); break;
                case ANSI_TERMINAL_COLOUR_BLUE: vga_text_set_attrib(attrib | VGA_TEXT_FG_LIGHT_BLUE); break;
                case ANSI_TERMINAL_COLOUR_MAGENTA: vga_text_set_attrib(attrib | VGA_TEXT_FG_LIGHT_MAGENTA); break;
                case ANSI_TERMINAL_COLOUR_CYAN: vga_text_set_attrib(attrib | VGA_TEXT_FG_LIGHT_CYAN); break;
                case ANSI_TERMINAL_COLOUR_WHITE: vga_text_set_attrib(attrib | VGA_TEXT_FG_WHITE); break;
                default: break;
            }
            break;
        default:
            switch (colour) {
                case ANSI_TERMINAL_COLOUR_BLACK: vga_text_set_attrib(attrib | VGA_TEXT_FG_BLACK); break;
                case ANSI_TERMINAL_COLOUR_RED: vga_text_set_attrib(attrib | VGA_TEXT_FG_RED); break;
                case ANSI_TERMINAL_COLOUR_GREEN: vga_text_set_attrib(attrib | VGA_TEXT_FG_GREEN); break;
                case ANSI_TERMINAL_COLOUR_YELLOW: vga_text_set_attrib(attrib | VGA_TEXT_FG_BROWN); break;
                case ANSI_TERMINAL_COLOUR_BLUE: vga_text_set_attrib(attrib | VGA_TEXT_FG_BLUE); break;
                case ANSI_TERMINAL_COLOUR_MAGENTA: vga_text_set_attrib(attrib | VGA_TEXT_FG_MAGENTA); break;
                case ANSI_TERMINAL_COLOUR_CYAN: vga_text_set_attrib(attrib | VGA_TEXT_FG_CYAN); break;
                case ANSI_TERMINAL_COLOUR_WHITE: vga_text_set_attrib(attrib | VGA_TEXT_FG_GREY); break;
                default: break;
            }
            break;
    }
}

void vga_text_ansi_set_background(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier) {
    (void)user;
    uint8_t attrib = vga_text_get_attrib() & ~VGA_TEXT_BG_MASK;
    switch (modifier) {
        case ANSI_TERMINAL_COLOUR_BRIGHT:
            switch (colour) {
                case ANSI_TERMINAL_COLOUR_BLACK: vga_text_set_attrib(attrib | VGA_TEXT_BG_DARK_GREY); break;
                case ANSI_TERMINAL_COLOUR_RED: vga_text_set_attrib(attrib | VGA_TEXT_BG_LIGHT_RED); break;
                case ANSI_TERMINAL_COLOUR_GREEN: vga_text_set_attrib(attrib | VGA_TEXT_BG_LIGHT_GREEN); break;
                case ANSI_TERMINAL_COLOUR_YELLOW: vga_text_set_attrib(attrib | VGA_TEXT_BG_YELLOW); break;
                case ANSI_TERMINAL_COLOUR_BLUE: vga_text_set_attrib(attrib | VGA_TEXT_BG_LIGHT_BLUE); break;
                case ANSI_TERMINAL_COLOUR_MAGENTA: vga_text_set_attrib(attrib | VGA_TEXT_BG_LIGHT_MAGENTA); break;
                case ANSI_TERMINAL_COLOUR_CYAN: vga_text_set_attrib(attrib | VGA_TEXT_BG_LIGHT_CYAN); break;
                case ANSI_TERMINAL_COLOUR_WHITE: vga_text_set_attrib(attrib | VGA_TEXT_BG_WHITE); break;
                default: break;
            }
            break;
        default:
            switch (colour) {
                case ANSI_TERMINAL_COLOUR_BLACK: vga_text_set_attrib(attrib | VGA_TEXT_BG_BLACK); break;
                case ANSI_TERMINAL_COLOUR_RED: vga_text_set_attrib(attrib | VGA_TEXT_BG_RED); break;
                case ANSI_TERMINAL_COLOUR_GREEN: vga_text_set_attrib(attrib | VGA_TEXT_BG_GREEN); break;
                case ANSI_TERMINAL_COLOUR_YELLOW: vga_text_set_attrib(attrib | VGA_TEXT_BG_BROWN); break;
                case ANSI_TERMINAL_COLOUR_BLUE: vga_text_set_attrib(attrib | VGA_TEXT_BG_BLUE); break;
                case ANSI_TERMINAL_COLOUR_MAGENTA: vga_text_set_attrib(attrib | VGA_TEXT_BG_MAGENTA); break;
                case ANSI_TERMINAL_COLOUR_CYAN: vga_text_set_attrib(attrib | VGA_TEXT_BG_CYAN); break;
                case ANSI_TERMINAL_COLOUR_WHITE: vga_text_set_attrib(attrib | VGA_TEXT_BG_GREY); break;
                default: break;
            }
            break;
    }
}

static ansi_terminal_t vga_text_ansi_terminal_emulation = {
    .user = NULL,

    .read = NULL,
    .write = vga_text_term_write,
    .read_available = NULL,
    .write_available = NULL,

    .move_cursor = vga_text_ansi_move_cursor,
    .set_cursor = vga_text_ansi_set_cursor,
    .clear_line = vga_text_ansi_clear_line,
    .clear_line_left = vga_text_ansi_clear_line_left,
    .clear_line_right = vga_text_ansi_clear_line_right,
    .clear_lines_before = vga_text_ansi_clear_lines_before,
    .clear_lines_after = vga_text_ansi_clear_lines_after,
    .clear_screen = vga_text_ansi_clear_screen,
    .reset_foreground = NULL,
    .reset_background = NULL,
    .set_foreground_8 = vga_text_ansi_set_foreground,
    .set_background_8 = vga_text_ansi_set_background,
    .set_foreground_256 = NULL,
    .set_background_256 = NULL,
    .set_foreground_rgb = NULL,
    .set_background_rgb = NULL,

    .state = ANSI_TERMINAL_STATE_IDLE,
    .param_buf = {0},
    .param_idx = 0
};

terminal_t vga_text_ansi_terminal = ANSI_TERMINAL_INTERFACE(&vga_text_ansi_terminal_emulation);

void vga_text_init_terminal(bool ansi) {
    term_init((ansi) ? &vga_text_ansi_terminal : &vga_text_terminal);
}
