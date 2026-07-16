#include <drivers/uart/16550.h>

bool uart_16550_is_tx_full(const uart_16550_t* driver) {
    return !(UART_16550_LSR_READ(driver) & UART_16550_LINESTAT_TX_NFULL);
}

bool uart_16550_is_tx_empty(const uart_16550_t* driver) {
    return (UART_16550_LSR_READ(driver) & UART_16550_LINESTAT_TX_EMPTY);
}

void uart_16550_transmit(const uart_16550_t* driver, uint8_t byte) {
    UART_16550_DR_WRITE(driver, byte);
}
