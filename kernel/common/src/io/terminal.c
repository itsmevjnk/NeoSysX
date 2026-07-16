#include <io/terminal.h>

static terminal_t* term_current = NULL;

void term_init(terminal_t* interface) {
    term_current = interface;
}

void term_putc(char c) {
    if (!term_current || !term_current->write) return;
    if (term_current->write_available) {
        while (!term_current->write_available(term_current->user)); // TODO: yield
    }
    term_current->write(term_current->user, c);
}

char term_getc(void) {
    if (!term_current || !term_current->read || !term_current->read_available) return 0;
    while (!term_current->read_available(term_current->user)); // TODO: yield
    return term_current->read(term_current->user);
}

bool term_try_putc(char c) {
    if (!term_current || !term_current->write) return false;
    if (term_current->write_available && !term_current->write_available(term_current->user)) return false;
    term_current->write(term_current->user, c);
    return true;
}

bool term_try_getc(char* c_out) {
    if (!term_current || !term_current->read || !term_current->read_available) return false;
    if (!term_current->read_available(term_current->user)) return false;
    *c_out = term_current->read(term_current->user);
    return true;
}

bool term_read_available(void) {
    if (!term_current || !term_current->read_available) return false;
    return term_current->read_available(term_current->user);
}

bool term_write_available(void) {
    if (!term_current) return false;
    if (!term_current->write_available) return true;
    return term_current->write_available(term_current->user);
}

void term_puts(const char* str) {
    if (!term_current) return;
    for (size_t i = 0; str[i]; i++)
        term_putc(str[i]);
}

void term_putbuf(const char* buf, size_t length) {
    if (!term_current) return;
    for (size_t i = 0; i < length; i++)
        term_putc(buf[i]);
}

size_t term_try_puts(const char* str) {
    if (!term_current) return 0;
    size_t sent = 0;
    for (; str[sent]; sent++) {
        if (!term_try_putc(str[sent])) break;
    }
    return sent;
}

size_t term_try_putbuf(const char* buf, size_t length) {
    if (!term_current) return 0;
    size_t sent = 0;
    for (; sent < length; sent++) {
        if (!term_try_putc(buf[sent])) break;
    }
    return sent;
}