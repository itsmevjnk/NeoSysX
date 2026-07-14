#include <string.h>

__attribute__((weak))
const char* strrchr(const char* str, int ch) {
    size_t len = strlen(str);
    if (!len) return NULL;

    for (size_t i = len - 1; /* termination condition implemented below */ ; i--) {
        if (str[i] == ch) {
            return &str[i];
        }
        if (!i) {
            break;
        }
    }
    return NULL;
}