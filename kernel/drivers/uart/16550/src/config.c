#include <drivers/uart/16550.h>

int32_t uart_16550_get_baud(const uart_16550_t* driver, uint16_t divisor) {
    if (!divisor) return -1; // invalid baud rate
    uint32_t baud = driver->baud_clk / divisor;
    if ((driver->baud_clk % divisor) >= (driver->baud_clk >> 1)) baud++; // round up
    return baud;
}

uint32_t uart_16550_set_baud(const uart_16550_t* driver, uint32_t baud) {
    if (baud > driver->baud_clk) baud = driver->baud_clk;
    
    uint32_t divisor; // NOTE: only the lower 16 bits are used
    if (!baud) divisor = 0xFFFF; // minimum baud rate
    else {
        divisor = driver->baud_clk / baud;
        if (divisor & ~0xFFFF) divisor = 0xFFFF;
    }
    
    UART_16550_LCR_WRITE(driver, UART_16550_LCR_READ(driver) | (1 << 7)); // enable DLAB
    UART_16550_DR_WRITE(driver, ((divisor >> 0) & 0xFF)); // LSB
    UART_16550_IER_WRITE(driver, ((divisor >> 8) & 0xFF)); // MSB
    UART_16550_LCR_WRITE(driver, UART_16550_LCR_READ(driver) & ~(1 << 7)); // disable DLAB

    return uart_16550_get_baud(driver, divisor);
}

void uart_16550_configure_bits(const uart_16550_t* driver, uart_16550_data_bits_t data, uart_16550_stop_bits_t stop, uart_16550_parity_t parity) {
    uint8_t byte = 0;
    switch (data) {
        case UART_16550_DATA_5_BITS: byte |= (0 << 0); break;
        case UART_16550_DATA_6_BITS: byte |= (1 << 0); break;
        case UART_16550_DATA_7_BITS: byte |= (2 << 0); break;
        // case UART_16550_DATA_8_BITS: byte |= (3 << 0); break;
        default: byte |= (3 << 0); break;
    }
    if (stop == UART_16550_STOP_2_BITS) byte |= (1 << 2);
    switch (parity) {
        // case UART_16550_PARITY_NONE:  byte |= (0 << 3); break;
        case UART_16550_PARITY_ODD:   byte |= (0b001 << 3); break;
        case UART_16550_PARITY_EVEN:  byte |= (0b011 << 3); break;
        case UART_16550_PARITY_MARK:  byte |= (0b101 << 3); break;
        case UART_16550_PARITY_SPACE: byte |= (0b111 << 3); break;
        default: break;
    }
    UART_16550_LCR_WRITE(driver, byte); // NOTE: this will also clear DLAB and break
}

int32_t uart_16550_init(const uart_16550_t* driver, const uart_16550_init_t* init) {
    if (!uart_16550_probe(driver)) return -1; // UART 16550 interface is invalid
    
    uint32_t baud = uart_16550_set_baud(driver, init->baud);
    uart_16550_configure_bits(driver, init->data_bits, init->stop_bits, init->parity_bits);
    
    if (init->fifo_enabled) uart_16550_disable_fifo(driver);
    else uart_16550_enable_fifo(driver, init->fifo_rx_level, init->fifo_dma_mode);
    
    uart_16550_set_interrupts(driver, init->interrupts);
    
    if (!uart_16550_test_loopback(driver)) return -2; // loopback test failed
    
    return baud;
}
