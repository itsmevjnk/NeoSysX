#include <drivers/uart/16550.h>

void uart_16550_set_break(const uart_16550_t* driver, bool state) {
    if (state) UART_16550_LCR_WRITE(driver, UART_16550_LCR_READ(driver) | (1 << 6));
    else UART_16550_LCR_WRITE(driver, UART_16550_LCR_READ(driver) & ~(1 << 6));
}
