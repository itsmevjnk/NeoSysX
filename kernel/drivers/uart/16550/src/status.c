#include <drivers/uart/16550.h>

uint8_t uart_16550_get_modem_status(const uart_16550_t* driver) {
    return UART_16550_MSR_READ(driver);
}

uint8_t uart_16550_get_line_status(const uart_16550_t* driver) {
    return UART_16550_LSR_READ(driver);
}
