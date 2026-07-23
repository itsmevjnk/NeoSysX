#ifndef __DRIVERS_VIDEO_VGA_TEXT_H
#define __DRIVERS_VIDEO_VGA_TEXT_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <io/terminal.h>
#include <io/ansi_terminal.h>

extern uint16_t* vga_text_buffer;

#define VGA_TEXT_WIDTH                      80
#define VGA_TEXT_HEIGHT                     25

#define VGA_TEXT_FG_BLACK                   (0 << 0)
#define VGA_TEXT_FG_BLUE                    (1 << 0)
#define VGA_TEXT_FG_GREEN                   (2 << 0)
#define VGA_TEXT_FG_CYAN                    (3 << 0)
#define VGA_TEXT_FG_RED                     (4 << 0)
#define VGA_TEXT_FG_MAGENTA                 (5 << 0)
#define VGA_TEXT_FG_BROWN                   (6 << 0)
#define VGA_TEXT_FG_GREY                    (7 << 0)
#define VGA_TEXT_FG_DARK_GREY               (8 << 0)
#define VGA_TEXT_FG_LIGHT_BLUE              (9 << 0)
#define VGA_TEXT_FG_LIGHT_GREEN             (10 << 0)
#define VGA_TEXT_FG_LIGHT_CYAN              (11 << 0)
#define VGA_TEXT_FG_LIGHT_RED               (12 << 0)
#define VGA_TEXT_FG_LIGHT_MAGENTA           (13 << 0)
#define VGA_TEXT_FG_YELLOW                  (14 << 0)
#define VGA_TEXT_FG_WHITE                   (15 << 0)

#define VGA_TEXT_FG_MASK                    (0b1111 << 0)

#define VGA_TEXT_BG_BLACK                   (0 << 4)
#define VGA_TEXT_BG_BLUE                    (1 << 4)
#define VGA_TEXT_BG_GREEN                   (2 << 4)
#define VGA_TEXT_BG_CYAN                    (3 << 4)
#define VGA_TEXT_BG_RED                     (4 << 4)
#define VGA_TEXT_BG_MAGENTA                 (5 << 4)
#define VGA_TEXT_BG_BROWN                   (6 << 4)
#define VGA_TEXT_BG_GREY                    (7 << 4)

/* NOTE: only available if bg16 == true in vga_text_init */
#define VGA_TEXT_BG_DARK_GREY               (8 << 4)
#define VGA_TEXT_BG_LIGHT_BLUE              (9 << 4)
#define VGA_TEXT_BG_LIGHT_GREEN             (10 << 4)
#define VGA_TEXT_BG_LIGHT_CYAN              (11 << 4)
#define VGA_TEXT_BG_LIGHT_RED               (12 << 4)
#define VGA_TEXT_BG_LIGHT_MAGENTA           (13 << 4)
#define VGA_TEXT_BG_YELLOW                  (14 << 4)
#define VGA_TEXT_BG_WHITE                   (15 << 4)

#define VGA_TEXT_BG_MASK                    (0b1111 << 4)

#define VGA_TEXT_BLINK                      (1 << 7) // only available if bg16 == false in vga_text_init

void vga_text_init(void* buffer_ptr, bool bg16);
void vga_text_clear(void);
void vga_text_scroll_up(uint8_t lines);
void vga_text_putc_at(uint8_t x, uint8_t y, char c, uint8_t attrib);

/* teletype functionality */
void vga_text_sync_cursor(void); // synchronise cursor position in memory with that currently set in hardware; called in init
void vga_text_update_cursor(void); // update cursor in hardware with current position in memory
void vga_text_set_cursor(uint8_t x, uint8_t y);
void vga_text_get_cursor(uint8_t* x, uint8_t* y);
void vga_text_set_attrib(uint8_t attrib);
uint8_t vga_text_get_attrib(void);
void vga_text_putc(char c);

/* terminal emulation */
void vga_text_term_write(void* user, char c);
extern terminal_t vga_text_terminal;
void vga_text_ansi_move_cursor(void* user, int dx, int dy);
void vga_text_ansi_set_cursor(void* user, int x, int y);
void vga_text_ansi_clear_line(void* user);
void vga_text_ansi_clear_line_left(void* user);
void vga_text_ansi_clear_line_right(void* user);
void vga_text_ansi_clear_lines_before(void* user);
void vga_text_ansi_clear_lines_after(void* user);
void vga_text_ansi_clear_screen(void* user);
void vga_text_ansi_set_foreground(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier);
void vga_text_ansi_set_background(void* user, ansi_terminal_colour8_t colour, ansi_terminal_colour8_modifier_t modifier);
extern terminal_t vga_text_ansi_terminal;
void vga_text_init_terminal(bool ansi);

#endif /* __DRIVERS_VIDEO_VGA_TEXT_H */