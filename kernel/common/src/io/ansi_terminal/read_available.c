#include <io/ansi_terminal.h>

bool ansi_terminal_read_available(void* user) {
    ansi_terminal_t* term = (ansi_terminal_t*) user;
    if (!term->read_available) return false;
    return term->read_available(term->user);
}