#ifndef __DRIVERS_UART_16550_PC_H
#define __DRIVERS_UART_16550_PC_H

#include <drivers/uart/16550.h>
#include <io/terminal.h>

uint8_t uart_16550_pc_read(uintptr_t reg);
void uart_16550_pc_write(uintptr_t reg, uint8_t data);

#define UART_16550_PC(base) UART_16550(base, 1, uart_16550_pc_read, uart_16550_pc_write, 115200)

extern uint16_t uart_16550_pc_bases[4]; // I/O port base addresses for COM1 to COM4
// NOTE: this is pre-populated with defaults; however, the BDA can be parsed to populate this with information from the BIOS

extern uart_16550_t uart_16550_pc_drivers[4];
extern uart_16550_t* uart_16550_pc_default; // NULL if there are no ports
extern terminal_t uart_16550_pc_terminal; // terminal interface using default interface (by default)

#define UART_16550_PC_COM1              (1 << 0)
#define UART_16550_PC_COM2              (1 << 1)
#define UART_16550_PC_COM3              (1 << 2)
#define UART_16550_PC_COM4              (1 << 3)
uint8_t uart_16550_pc_probe(void); // also populates uart_16550_pc_drivers and uart_16550_pc_default, as well as uart_16550_pc_interface

void uart_16550_pc_set_terminal_port(uint8_t port);
void uart_16550_pc_init_terminal(void); // initialise terminal using default UART interface (by default)

#endif /* __DRIVERS_UART_16550_PC_H */