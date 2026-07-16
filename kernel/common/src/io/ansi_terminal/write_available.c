#include <io/ansi_terminal.h>

bool ansi_terminal_write_available(void* user) {
    ansi_terminal_t* term = (ansi_terminal_t*) user;
    if (!term->write_available) return true;
    return term->write_available(term->user);
    // NOTE: technically writing might still be available if we're receiving escape sequence
}
