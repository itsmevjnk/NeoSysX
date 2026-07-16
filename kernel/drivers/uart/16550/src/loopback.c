#include <drivers/uart/16550.h>

void uart_16550_enable_loopback(const uart_16550_t* driver) {
    UART_16550_MCR_WRITE(driver, UART_16550_MCR_READ(driver) | (1 << 4));
}

void uart_16550_disable_loopback(const uart_16550_t* driver) {
    UART_16550_MCR_WRITE(driver, UART_16550_MCR_READ(driver) & ~(1 << 4));
}