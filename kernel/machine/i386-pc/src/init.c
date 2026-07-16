#include <machine/i386-pc/init.h>
#include <kernel/panic.h>

#include <cpu/i386/init.h>
#include <drivers/uart/16550_pc.h>

#include <io/terminal.h>

void kinit_machine_pre(void) {
    kinit_cpu_pre();

    if (!uart_16550_pc_probe()) kpanic();
    uart_16550_pc_init_terminal();
}

void kinit_machine_post(void) {
    kinit_cpu_post();
    
    term_puts("Hello, World!\n");
}
