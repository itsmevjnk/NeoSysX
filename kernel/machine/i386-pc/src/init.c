#include <machine/i386-pc/init.h>
#include <kernel/panic.h>
#include <kernel/log.h>
#include <cpu/intr.h>

#include <cpu/i386/init.h>
#include <drivers/intr/i8259.h>
#include <drivers/timer/i8253.h>
#include <drivers/uart/16550_pc.h>
#include <drivers/video/vga_text.h>

#ifndef PIT_DESIRED_TICK_FREQ
#define PIT_DESIRED_TICK_FREQ                           29102 // other good values are 41, 82, 14551 and 596591 (probably too high though)
#endif

void kinit_machine_pre(void) {
    kinit_cpu_pre();

    if (!uart_16550_pc_probe()) kpanic();
    // uart_16550_pc_init_terminal();

    vga_text_init((void*)0xC00B8000, true); // higher-half mapped VGA text buffer
    vga_text_init_terminal(true);
    
    pic_init();
    pit_init(PIT_DESIRED_TICK_FREQ);
    intr_enable();
}

void kinit_machine_post(void) {
    kinit_cpu_post();
    
    LOG_INFO("Hello, World!");
}
