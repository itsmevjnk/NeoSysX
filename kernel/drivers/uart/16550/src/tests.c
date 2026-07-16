#include <drivers/uart/16550.h>

#define UART_16550_TEST_BYTE                    0xAA

bool uart_16550_probe(const uart_16550_t* driver) {
    /* check if scratch register works */
    UART_16550_SR_WRITE(driver, UART_16550_TEST_BYTE);
    return UART_16550_SR_READ(driver) == UART_16550_TEST_BYTE;
}

bool uart_16550_test_loopback(const uart_16550_t* driver) {
    uart_16550_enable_loopback(driver); // enable loopback

    bool success = true;
    
    /* test TX/RX */
    uart_16550_transmit(driver, UART_16550_TEST_BYTE);
    success = uart_16550_receive(driver) == UART_16550_TEST_BYTE;
    if (!success) goto end;

    /* test modem control lines */
    for (uint8_t mask = (1 << 0); mask <= (1 << 3) && success; mask <<= 1) {
        uart_16550_set_modem_lines(driver, mask);
        success = (uart_16550_get_modem_status(driver) >> 4) == mask;
    }
    uart_16550_set_modem_lines(driver, 0);

end:
    uart_16550_disable_loopback(driver); // finally disable loopback
    return success;
}


