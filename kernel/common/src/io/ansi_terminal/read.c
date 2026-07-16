#include <io/ansi_terminal.h>

char ansi_terminal_read(void* user) {
    ansi_terminal_t* term = (ansi_terminal_t*) user;
    if (!term->read) return 0;
    return term->read(term->user);
}
