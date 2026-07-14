#include <string.h>

__attribute__((weak))
const void* memchr(const void* ptr, int ch, size_t count) {
    const char* ptr_char = (const char*) ptr;
    for (size_t i = 0; i < count; i++) {
        if (ptr_char[i] == ch) {
            return (const void*) &ptr_char[i];
        }
    }
    return NULL;
}