#ifndef __DRIVERS_UART_16550_TERM_H
#define __DRIVERS_UART_16550_TERM_H

#include <drivers/uart/16550.h>
#include <io/terminal.h>

char uart_16550_term_read(void* user);
void uart_16550_term_write(void* user, char c);
bool uart_16550_term_read_available(void* user);
bool uart_16550_term_write_available(void* user);

#define UART_16550_TERM(driver) (terminal_t){\
    .user = (void*)(driver), \
    .read = uart_16550_term_read, \
    .write = uart_16550_term_write, \
    .read_available = uart_16550_term_read_available, \
    .write_available = uart_16550_term_write_available \
}

#endif /* __DRIVERS_UART_16550_TERM_H */