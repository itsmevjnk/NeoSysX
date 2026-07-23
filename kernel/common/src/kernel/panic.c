#include <kernel/panic.h>
#include <kernel/log.h>
#include <cpu/halt.h>

__attribute__((noreturn)) void kpanic(void) {
    // LOG_DEBUG("stack trace:");
    // ktrace();

    LOG_ERROR("kpanic() invoked");
    while (1) {
        cpu_halt();
    }
}