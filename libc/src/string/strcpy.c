#include <string.h>

__attribute__((weak))
char* strcpy(char* dest, const char* src) {
    // NOTE: we can instead call strncpy(dest, src, SIZE_MAX)
    for (size_t i = 0; ; i++) {
        dest[i] = src[i];
        if (!src[i]) break;
    }
    return dest;
}