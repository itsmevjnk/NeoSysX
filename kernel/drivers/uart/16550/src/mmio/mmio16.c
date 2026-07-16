#include <drivers/uart/16550.h>

uint8_t uart_16550_mmio16_read(uintptr_t reg) {
    return *((volatile uint16_t*)reg);
}

void uart_16550_mmio16_write(uintptr_t reg, uint8_t data) {
    *((volatile uint16_t*)reg) = data;
}
