#include <drivers/uart/16550.h>

void uart_16550_disable_fifo(const uart_16550_t* driver) {
    UART_16550_FCR_WRITE(driver, 0);
}

void uart_16550_enable_fifo(const uart_16550_t* driver, uart_16550_fifo_level_t rx_level, bool dma_mode) {
    uint8_t byte = (1 << 0) | ((dma_mode) ? (1 << 3) : 0) | (1 << 1) | (1 << 2); // clear RX/TX FIFO too
    switch (rx_level) {
        // case UART_16550_FIFO_1_BYTE: byte |= (0 << 6); break;
        case UART_16550_FIFO_4_BYTES: byte |= (1 << 6); break;
        case UART_16550_FIFO_8_BYTES: byte |= (2 << 6); break;
        case UART_16550_FIFO_14_BYTES: byte |= (3 << 6); break;
        default: break;
    }
    UART_16550_FCR_WRITE(driver, byte);
}

