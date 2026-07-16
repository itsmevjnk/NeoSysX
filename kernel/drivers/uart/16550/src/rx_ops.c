#include <drivers/uart/16550.h>

bool uart_16550_is_rx_available(const uart_16550_t* driver) {
    return (UART_16550_LSR_READ(driver) & UART_16550_LINESTAT_RX_READY);
}

uint8_t uart_16550_receive(const uart_16550_t* driver) {
    return UART_16550_DR_READ(driver);
}
