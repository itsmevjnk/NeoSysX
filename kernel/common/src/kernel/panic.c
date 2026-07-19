#include <kernel/panic.h>
#include <kernel/log.h>
void kpanic(void) {
    // LOG_DEBUG("stack trace:");
    // ktrace();
    while (1);
}