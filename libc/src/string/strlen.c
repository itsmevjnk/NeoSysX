#include <string.h>

__attribute__((weak))
size_t strlen(const char* str) {
    // NOTE: we can instead call memchr(str, 0, SIZE_MAX)
    size_t len = 0;
    for (; str[len]; len++);
    return len;
}