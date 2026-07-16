#include <stdio.h>
#include <io/terminal.h>

int putchar(int ch) {
    term_putc(ch);
    return ch;
}
