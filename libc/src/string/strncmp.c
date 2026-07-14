#include <string.h>

__attribute__((weak))
int strncmp(const char* lhs, const char* rhs, size_t count) {
    int ret = 0;
    for (size_t i = 0; (lhs[i] || rhs[i]) && !ret && i < count; i++) {
        ret = lhs[i] - rhs[i];
    }
    return ret;
}