#include <string.h>

__attribute__((weak))
char* strncpy(char* dest, const char* src, size_t count) {
    for (size_t i = 0; i < count; i++) {
        dest[i] = src[i];
        if (!src[i]) break;
    }
    return dest;
}