#include <drivers/uart/16550_term.h>

char uart_16550_term_read(void* user) {
    return (char)uart_16550_receive((uart_16550_t*)user);
}

void uart_16550_term_write(void* user, char c) {
    uart_16550_transmit((uart_16550_t*)user, (uint8_t)c);
}

bool uart_16550_term_read_available(void* user) {
    return uart_16550_is_rx_available((uart_16550_t*)user);
}

bool uart_16550_term_write_available(void* user) {
    return !uart_16550_is_tx_full((uart_16550_t*)user);
}
