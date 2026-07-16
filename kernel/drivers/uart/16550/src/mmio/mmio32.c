#include <drivers/uart/16550.h>

uint8_t uart_16550_mmio32_read(uintptr_t reg) {
    return *((volatile uint32_t*)reg);
}

void uart_16550_mmio32_write(uintptr_t reg, uint8_t data) {
    *((volatile uint32_t*)reg) = data;
}
