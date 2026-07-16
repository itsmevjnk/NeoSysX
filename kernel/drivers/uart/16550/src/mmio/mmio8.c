#include <drivers/uart/16550.h>

uint8_t uart_16550_mmio8_read(uintptr_t reg) {
    return *((volatile uint8_t*)reg);
}

void uart_16550_mmio8_write(uintptr_t reg, uint8_t data) {
    *((volatile uint8_t*)reg) = data;
}
