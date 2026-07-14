#include <string.h>

__attribute__((weak))
const char* strchr(const char* str, int ch) {
    for (size_t i = 0; str[i]; i++) {
        if (str[i] == ch) {
            return &str[i];
        }
    }
    return NULL;
}