#include <string.h>

__attribute__((weak))
char* strcat(char* dest, const char* src) {
    // NOTE: we can instead call strncat(dest, src, SIZE_MAX)
    size_t dest_len = strlen(dest);
    for (size_t i = 0; ; i++) {
        dest[dest_len + i] = src[i];
        if (!src[i]) break;
    }
    return dest;
}