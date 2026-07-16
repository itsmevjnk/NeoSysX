/* kernel (ANSI) terminal interface */

#ifndef __IO_TERMINAL_H
#define __IO_TERMINAL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* terminal interface structure */
typedef struct {
    void* user;

    // NOTE: must be implemented as non-blocking
    char (*read)(void* user); 
    void (*write)(void* user, char c);

    bool (*read_available)(void* user);
    bool (*write_available)(void* user);
} terminal_t;

void term_init(terminal_t* interface); // set terminal interface

// synchronous interface - block until successful
void term_putc(char c);
char term_getc(void);
void term_puts(const char* str);
void term_putbuf(const char* buf, size_t length);

// asynchronous interface - return true if successful, false otherwise
bool term_try_putc(char c);
bool term_try_getc(char* c_out);
bool term_read_available(void);
bool term_write_available(void);
// these functions return the number of characters sent
size_t term_try_puts(const char* str);
size_t term_try_putbuf(const char* buf, size_t length);

#endif /* __IO_TERMINAL_H */