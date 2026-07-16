#include <machine/i386-pc/init.h>
#include <panic.h>

#include <cpu/i386/init.h>
#include <drivers/uart/16550_pc.h>

void kinit_machine_pre(void) {
    kinit_cpu_pre();

    if (!uart_16550_pc_probe()) kpanic();
}

void kinit_machine_post(void) {
    kinit_cpu_post();

    uart_16550_transmit(uart_16550_pc_default, 'h');
}
