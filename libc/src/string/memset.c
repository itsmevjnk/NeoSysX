#include <string.h>

__attribute__((weak))
void* memset(void* dest, int ch, size_t count) {
    char* dest_char = (char*) dest;
    for (size_t i = 0; i < count; i++) {
        dest_char[i] = ch;
    }    
    return dest;
}