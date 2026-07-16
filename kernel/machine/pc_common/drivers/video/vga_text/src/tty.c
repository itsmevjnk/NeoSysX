#include <drivers/video/vga_text.h>
#include <cpu/x86_common/io.h>

static uint8_t vga_text_tty_x = 0, vga_text_tty_y = 0;
static uint8_t vga_text_tty_attrib = VGA_TEXT_BG_BLACK | VGA_TEXT_FG_GREY;

void vga_text_sync_cursor(void) {
    uint16_t pos = 0;
    outb(0x3D4, 0x0F);
    pos |= inb(0x3D5);
    outb(0x3D4, 0x0E);
    pos |= (uint16_t)inb(0x3D5) << 8;
    vga_text_tty_y = pos / VGA_TEXT_WIDTH;
    vga_text_tty_x = pos % VGA_TEXT_WIDTH;
}

void vga_text_update_cursor(void) {
    // TODO: only update cursor once in a while
    uint16_t pos = vga_text_tty_y * VGA_TEXT_WIDTH + vga_text_tty_x;
    outb(0x3D4, 0x0F); outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E); outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void vga_text_set_cursor(uint8_t x, uint8_t y) {
    if (x >= VGA_TEXT_WIDTH || y >= VGA_TEXT_HEIGHT) return;
    vga_text_tty_x = x; vga_text_tty_y = y;

    vga_text_update_cursor();
}

void vga_text_get_cursor(uint8_t* x, uint8_t* y) {
    if (x) *x = vga_text_tty_x;
    if (y) *y = vga_text_tty_y;
}

void vga_text_set_attrib(uint8_t attrib) {
    vga_text_tty_attrib = attrib;
}

uint8_t vga_text_get_attrib(void) {
    return vga_text_tty_attrib;
}

#ifndef VGA_TEXT_TAB_WIDTH
#define VGA_TEXT_TAB_WIDTH 8
#endif /* VGA_TEXT_CURSOR_END */

void vga_text_putc(char c) {
    int tmp;
    switch (c) {
        case '\b': // backspace
            if (vga_text_tty_x == 0) {
                if (vga_text_tty_y) {
                    vga_text_tty_y--;
                    vga_text_tty_x = VGA_TEXT_WIDTH - 1;
                }
            } else vga_text_tty_x--;
            break;
        case '\n': // line feed (also acts as carriage return here)
            vga_text_tty_x = 0;
            if (vga_text_tty_y == VGA_TEXT_HEIGHT - 1) vga_text_scroll_up(1);
            else vga_text_tty_y++;
            break;
        case '\r': // carriage return - already handled above
            break;
        case '\t': // horizontal tab
            tmp = VGA_TEXT_TAB_WIDTH - vga_text_tty_x % VGA_TEXT_TAB_WIDTH;
            while (tmp--) vga_text_putc(' ');
            break;
        default:
            vga_text_putc_at(vga_text_tty_x, vga_text_tty_y, c, vga_text_tty_attrib);
            vga_text_tty_x++;
            if (vga_text_tty_x == VGA_TEXT_WIDTH) {
                vga_text_tty_x = 0;
                if (vga_text_tty_y == VGA_TEXT_HEIGHT - 1) vga_text_scroll_up(1);
                else vga_text_tty_y++;
            }
            break;
    }
    vga_text_update_cursor();
}
