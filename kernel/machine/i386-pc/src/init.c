#include <machine/i386-pc/init.h>
#include <kernel/panic.h>
#include <kernel/log.h>
#include <cpu/intr.h>

#include <cpu/i386/init.h>
#include <drivers/intr/i8259.h>
#include <drivers/uart/16550_pc.h>
#include <drivers/video/vga_text.h>

void kinit_machine_pre(void) {
    kinit_cpu_pre();
    
    pic_init();
    // pic_unmask_irq(1 << 0); // NOTE: we can intentionally unmask IRQ 0 here so we can see unhandled IRQ warnings
    intr_enable();

    if (!uart_16550_pc_probe()) kpanic();
    // uart_16550_pc_init_terminal();

    vga_text_init(true);
    vga_text_init_terminal(true);
}

void kinit_machine_post(void) {
    kinit_cpu_post();
    
    LOG_INFO("Hello, World!");
}
