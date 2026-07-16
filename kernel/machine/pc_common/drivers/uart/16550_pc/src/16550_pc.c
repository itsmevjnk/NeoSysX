#include <drivers/uart/16550_pc.h>
#include <cpu/x86_common/io.h>

uint8_t uart_16550_pc_read(uintptr_t reg) {
    return inb(reg);
}

void uart_16550_pc_write(uintptr_t reg, uint8_t data) {
    outb(reg, data);
}

uint16_t uart_16550_pc_bases[4] = { 0x3F8, 0x2F8, 0x3E8, 0x2E8 };

uart_16550_t uart_16550_pc_drivers[4];
uart_16550_t* uart_16550_pc_default = NULL;

uint8_t uart_16550_pc_probe(void) {
    uart_16550_pc_default = NULL;

    uint8_t available_ports = 0;
    for (int port = 0; port < 4; port++) {
        if (!uart_16550_pc_bases[port]) continue; // invalid I/O base

        uart_16550_pc_drivers[port] = UART_16550_PC(uart_16550_pc_bases[port]);
        if (!uart_16550_probe(&uart_16550_pc_drivers[port])) continue; // probing failed
        
        available_ports |= (1 << port);
        if (!uart_16550_pc_default) uart_16550_pc_default = &uart_16550_pc_drivers[port];
    }

    return available_ports;
}
