#include <drivers/video/vga_text.h>
#include <cpu/x86_common/io.h>
#include <string.h>

volatile uint16_t* vga_text_buffer = (volatile uint16_t*)0xB8000;
// TODO: change this when we eventually move to higher half

void vga_text_clear(void) {
    memset((void*)vga_text_buffer, 0, VGA_TEXT_WIDTH * VGA_TEXT_HEIGHT * 2);
}

void vga_text_scroll_up(uint8_t lines) {
    memmove((void*)vga_text_buffer, (void*)&vga_text_buffer[lines * VGA_TEXT_WIDTH], (VGA_TEXT_HEIGHT - lines) * VGA_TEXT_WIDTH * 2);
    memset((void*)&vga_text_buffer[(VGA_TEXT_HEIGHT - lines) * VGA_TEXT_WIDTH], 0, lines * VGA_TEXT_WIDTH * 2);
}

void vga_text_putc_at(uint8_t x, uint8_t y, char c, uint8_t attrib) {
    vga_text_buffer[y * VGA_TEXT_WIDTH + x] = (uint16_t)c | ((uint16_t)attrib << 8);
}

#ifndef VGA_TEXT_CURSOR_START
#define VGA_TEXT_CURSOR_START 14
#endif /* VGA_TEXT_CURSOR_START */

#ifndef VGA_TEXT_CURSOR_END
#define VGA_TEXT_CURSOR_END 15
#endif /* VGA_TEXT_CURSOR_END */

void vga_text_init(bool bg16) {
    /* configure BLINK */
    inb(0x3DA);
    uint8_t adr_val = inb(0x3C0);
    outb(0x3C0, 0x10); // attribute mode control register
    uint8_t amcr_val = inb(0x3C1);
    if (bg16) amcr_val &= ~(1 << 3); else amcr_val |= (1 << 3);
    outb(0x3C0, amcr_val);
    outb(0x3C0, adr_val);
    inb(0x3DA);

    /* enable cursor (if it hadn't been enabled previously) */
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | VGA_TEXT_CURSOR_START);
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | VGA_TEXT_CURSOR_END);

    vga_text_sync_cursor();
}