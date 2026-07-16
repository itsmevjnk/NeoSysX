#include <drivers/uart/16550.h>

uint8_t uart_16550_mmio64_read(uintptr_t reg) {
    return *((volatile uint64_t*)reg);
}

void uart_16550_mmio64_write(uintptr_t reg, uint8_t data) {
    *((volatile uint64_t*)reg) = data;
}
