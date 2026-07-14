#include <string.h>

__attribute__((weak))
char* strncat(char* dest, const char* src, size_t count) {
    size_t dest_len = strlen(dest);
    for (size_t i = 0; i < count; i++) {
        dest[dest_len + i] = src[i];
        if (!src[i]) break;
    }
    return dest;
}