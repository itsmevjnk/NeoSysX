/* 16550 UART driver - broken out as there might be other platforms that want to use this */

#ifndef __DRIVERS_UART_16550_H
#define __DRIVERS_UART_16550_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* 16550 UART driver info struct - according to https://wiki.osdev.org/Serial_Ports */
typedef struct {
    /* register addresses (e.g. I/O address for x86, or MMIO address) */
    uintptr_t dr; // data register (DLAB=0) / baud rate divisor LSB (DLAB=1)
    uintptr_t ier; // interrupt enable register (DLAB=0) / baud rate divisor MSB (DLAB=1)
    uintptr_t fcr_isr; // FIFO control register (write) / interrupt status register (read)
    uintptr_t lcr; // line control register
    uintptr_t mcr; // modem control register
    uintptr_t lsr; // modem status register
    uintptr_t msr; // modem status register
    uintptr_t sr; // scratch pad register

    uint32_t baud_clk; // reference clock speed / 16 (Hz)

    /* read/write callbacks */
    uint8_t (*read_cb)(uintptr_t); // uint8_t read_cb(uintptr_t reg)
    void (*write_cb)(uintptr_t, uint8_t); // void write_cb(uintptr_t reg, uint8_t data)
} uart_16550_t;

/* I/O helpers */
#define UART_16550_DR_READ(driver)                      (driver->read_cb(driver->dr))
#define UART_16550_DR_WRITE(driver, value)              (driver->write_cb(driver->dr, (value)))
#define UART_16550_IER_READ(driver)                     (driver->read_cb(driver->ier))
#define UART_16550_IER_WRITE(driver, value)             (driver->write_cb(driver->ier, (value)))
#define UART_16550_ISR_READ(driver)                     (driver->read_cb(driver->fcr_isr))
#define UART_16550_FCR_WRITE(driver, value)             (driver->write_cb(driver->fcr_isr, (value)))
#define UART_16550_LCR_READ(driver)                     (driver->read_cb(driver->lcr))
#define UART_16550_LCR_WRITE(driver, value)             (driver->write_cb(driver->lcr, (value)))
#define UART_16550_MCR_READ(driver)                     (driver->read_cb(driver->mcr))
#define UART_16550_MCR_WRITE(driver, value)             (driver->write_cb(driver->mcr, (value)))
#define UART_16550_LSR_READ(driver)                     (driver->read_cb(driver->lsr))
#define UART_16550_MSR_READ(driver)                     (driver->read_cb(driver->msr))
#define UART_16550_SR_READ(driver)                      (driver->read_cb(driver->sr))
#define UART_16550_SR_WRITE(driver, value)              (driver->write_cb(driver->sr, (value)))

/* built-in read/write callbacks for MMIO */
uint8_t uart_16550_mmio8_read(uintptr_t reg);
void uart_16550_mmio8_write(uintptr_t reg, uint8_t data);
uint8_t uart_16550_mmio16_read(uintptr_t reg);
void uart_16550_mmio16_write(uintptr_t reg, uint8_t data);
uint8_t uart_16550_mmio32_read(uintptr_t reg);
void uart_16550_mmio32_write(uintptr_t reg, uint8_t data);
uint8_t uart_16550_mmio64_read(uintptr_t reg);
void uart_16550_mmio64_write(uintptr_t reg, uint8_t data);

#define UART_16550(base, stride, read, write, clk) (uart_16550_t){ \
    .dr = (base) + 0 * (stride), \
    .ier = (base) + 1 * (stride), \
    .fcr_isr = (base) + 2 * (stride), \
    .lcr = (base) + 3 * (stride), \
    .mcr = (base) + 4 * (stride), \
    .lsr = (base) + 5 * (stride), \
    .msr = (base) + 6 * (stride), \
    .sr = (base) + 7 * (stride), \
    .baud_clk = (clk), \
    .read_cb = (read), \
    .write_cb = (write), \
}

#define UART_16550_MMIO8(base, stride, clk) UART_16550(base, stride, uart_16550_mmio8_read, uart_16550_mmio8_write, clk)
#define UART_16550_MMIO8_DEFAULT(base, clk) UART_16550_MMIO8(base, 1, clk)
#define UART_16550_MMIO16(base, stride, clk) UART_16550(base, stride, uart_16550_mmio16_read, uart_16550_mmio16_write, clk)
#define UART_16550_MMIO16_DEFAULT(base, clk) UART_16550_MMIO16(base, 2, clk)
#define UART_16550_MMIO32(base, stride, clk) UART_16550(base, stride, uart_16550_mmio32_read, uart_16550_mmio32_write, clk)
#define UART_16550_MMIO32_DEFAULT(base, clk) UART_16550_MMIO32(base, 4, clk)

bool uart_16550_probe(const uart_16550_t* driver); // probe for existence of 16550 UART on the specified interface

int32_t uart_16550_get_baud(const uart_16550_t* driver, uint16_t divisor); // get baud rate given divisor
uint32_t uart_16550_set_baud(const uart_16550_t* driver, uint32_t baud); // set baud rate to closest possible to provided rate and return the actual rate

typedef enum {
    UART_16550_DATA_5_BITS = 5,
    UART_16550_DATA_6_BITS,
    UART_16550_DATA_7_BITS,
    UART_16550_DATA_8_BITS
} uart_16550_data_bits_t;
typedef enum {
    UART_16550_STOP_1_BIT = 1,
    UART_16550_STOP_2_BITS
} uart_16550_stop_bits_t;
typedef enum {
    UART_16550_PARITY_NONE,
    UART_16550_PARITY_ODD,
    UART_16550_PARITY_EVEN,
    UART_16550_PARITY_MARK,
    UART_16550_PARITY_SPACE
} uart_16550_parity_t;
void uart_16550_configure_bits(const uart_16550_t* driver, uart_16550_data_bits_t data, uart_16550_stop_bits_t stop, uart_16550_parity_t parity);

/* interrupt configuration */
#define UART_16550_INTMASK_RX_AVAILABLE             (1 << 0)
#define UART_16550_INTMASK_TX_EMPTY                 (1 << 1)
#define UART_16550_INTMASK_RX_LINE                  (1 << 2)
#define UART_16550_INTMASK_MODEM                    (1 << 3)
void uart_16550_set_interrupts(const uart_16550_t* driver, uint8_t mask);
void uart_16550_enable_interrupts(const uart_16550_t* driver, uint8_t mask);
void uart_16550_disable_interrupts(const uart_16550_t* driver, uint8_t mask);
void uart_16550_toggle_interrupts(const uart_16550_t* driver, uint8_t mask);

typedef enum {
    UART_16550_INTR_UNKNOWN,
    UART_16550_INTR_RX_LINE,
    UART_16550_INTR_RX_AVAILABLE,
    UART_16550_INTR_RX_TIMEOUT,
    UART_16550_INTR_TX_EMPTY,
    UART_16550_INTR_MODEM
} uart_16550_intr_t;
uart_16550_intr_t uart_16550_get_interrupt(const uart_16550_t* driver);

/* FIFO configuration */
void uart_16550_disable_fifo(const uart_16550_t* driver);
typedef enum {
    UART_16550_FIFO_1_BYTE = 1,
    UART_16550_FIFO_4_BYTES = 4,
    UART_16550_FIFO_8_BYTES = 8,
    UART_16550_FIFO_14_BYTES = 14
} uart_16550_fifo_level_t;
void uart_16550_enable_fifo(const uart_16550_t* driver, uart_16550_fifo_level_t rx_level, bool dma_mode);

/* line control */
void uart_16550_set_break(const uart_16550_t* driver, bool state);

/* modem control */
void uart_16550_enable_loopback(const uart_16550_t* driver);
void uart_16550_disable_loopback(const uart_16550_t* driver);
#define UART_16550_MODEMMASK_DTR                    (1 << 0)
#define UART_16550_MODEMMASK_RTS                    (1 << 1)
#define UART_16550_MODEMMASK_OUT1                   (1 << 2)
#define UART_16550_MODEMMASK_OUT2                   (1 << 3)
uint8_t uart_16550_get_modem_lines(const uart_16550_t* driver);
void uart_16550_set_modem_lines(const uart_16550_t* driver, uint8_t mask);
void uart_16550_enable_modem_lines(const uart_16550_t* driver, uint8_t mask);
void uart_16550_disable_modem_lines(const uart_16550_t* driver, uint8_t mask);
void uart_16550_toggle_modem_lines(const uart_16550_t* driver, uint8_t mask);
bool uart_16550_test_loopback(const uart_16550_t* driver);

/* modem status */
#define UART_16550_MODEMSTAT_CTS_DELTA              (1 << 0)
#define UART_16550_MODEMSTAT_DSR_DELTA              (1 << 1)
#define UART_16550_MODEMSTAT_RI_TRAIL               (1 << 2)
#define UART_16550_MODEMSTAT_DCD_DELTA              (1 << 3)
#define UART_16550_MODEMSTAT_CTS                    (1 << 4)
#define UART_16550_MODEMSTAT_DSR                    (1 << 5)
#define UART_16550_MODEMSTAT_RI                     (1 << 6)
#define UART_16550_MODEMSTAT_DCD                    (1 << 7)
uint8_t uart_16550_get_modem_status(const uart_16550_t* driver);

/* line status */
#define UART_16550_LINESTAT_RX_READY                (1 << 0)
#define UART_16550_LINESTAT_RX_OVERRUN              (1 << 1)
#define UART_16550_LINESTAT_PARITY_ERR              (1 << 2)
#define UART_16550_LINESTAT_FRAME_ERR               (1 << 3)
#define UART_16550_LINESTAT_RX_BREAK                (1 << 4)
#define UART_16550_LINESTAT_TX_NFULL                (1 << 5)
#define UART_16550_LINESTAT_TX_EMPTY                (1 << 6)
#define UART_16550_LINESTAT_RX_FIFO_ERR             (1 << 7)
uint8_t uart_16550_get_line_status(const uart_16550_t* driver);

/* general initialisation */
typedef struct {
    uint32_t baud;
    uart_16550_data_bits_t data_bits;
    uart_16550_stop_bits_t stop_bits;
    uart_16550_parity_t parity_bits;
    uint8_t interrupts;
    bool fifo_enabled;
    uart_16550_fifo_level_t fifo_rx_level;
    bool fifo_dma_mode;
} uart_16550_init_t;
#define UART_16550_INIT_DEFAULT {\
    .baud = 115200, \
    .data_bits = UART_16550_DATA_8_BITS, \
    .stop_bits = UART_16550_STOP_1_BIT, \
    .parity_bits = UART_16550_PARITY_NONE, \
    .interrupts = 0, \
    .fifo_enabled = true, \
    .fifo_rx_level = UART_16550_FIFO_14_BYTES, \
    .fifo_dma_mode = false \
}

int32_t uart_16550_init(const uart_16550_t* driver, const uart_16550_init_t* init);

/* receive/transmit */
bool uart_16550_is_tx_full(const uart_16550_t* driver);
bool uart_16550_is_tx_empty(const uart_16550_t* driver);
void uart_16550_transmit(const uart_16550_t* driver, uint8_t byte);
bool uart_16550_is_rx_available(const uart_16550_t* driver);
uint8_t uart_16550_receive(const uart_16550_t* driver);

#endif /* __DRIVERS_UART_16550_H */