#include <drivers/uart/16550.h>

void uart_16550_set_interrupts(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_IER_WRITE(driver, mask & 0b1111);
}

void uart_16550_enable_interrupts(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_IER_WRITE(driver, UART_16550_IER_READ(driver) | (mask & 0b1111));
}

void uart_16550_disable_interrupts(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_IER_WRITE(driver, UART_16550_IER_READ(driver) & ~(mask & 0b1111));
}

void uart_16550_toggle_interrupts(const uart_16550_t* driver, uint8_t mask) {
    UART_16550_IER_WRITE(driver, UART_16550_IER_READ(driver) ^ (mask & 0b1111));
}

uart_16550_intr_t uart_16550_get_interrupt(const uart_16550_t* driver) {
    uint8_t isr = UART_16550_ISR_READ(driver);
    switch (isr & 0b1111) {
        case 0b0110: return UART_16550_INTR_RX_LINE;
        case 0b0100: return UART_16550_INTR_RX_AVAILABLE;
        case 0b1100: return UART_16550_INTR_RX_TIMEOUT;
        case 0b0010: return UART_16550_INTR_TX_EMPTY;
        case 0b0000: return UART_16550_INTR_MODEM;
        default: return UART_16550_INTR_UNKNOWN;
    }
}
