#include <string.h>

__attribute__((weak))
size_t strnlen(const char* str, size_t maxlen) {
    // NOTE: we can instead call memchr(str, 0, maxlen)
    size_t len = 0;
    for (; str[len] && len < maxlen; len++);
    return len;
}