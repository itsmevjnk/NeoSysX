#include <drivers/uart/16550.h>

uint8_t uart_16550_get_modem_lines(const uart_16550_t* driver) {
    return UART_16550_MCR_READ(driver) & 0b1111;
}

void uart_16550_set_modem_lines(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_MCR_WRITE(driver, (UART_16550_MCR_READ(driver) & ~0b1111) | (mask & 0b1111));
}

void uart_16550_enable_modem_lines(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_MCR_WRITE(driver, UART_16550_MCR_READ(driver) | (mask & 0b1111));
}

void uart_16550_disable_modem_lines(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_MCR_WRITE(driver, UART_16550_MCR_READ(driver) & ~(mask & 0b1111));
}

void uart_16550_toggle_modem_lines(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_MCR_WRITE(driver, UART_16550_MCR_READ(driver) ^ (mask & 0b1111));
}
