#include <kernel/panic.h>
#include <kernel/log.h>
#include <cpu/halt.h>
#include <cpu/intr.h>

__attribute__((noreturn)) void kpanic(void) {
    // LOG_DEBUG("stack trace:");
    // ktrace();

    LOG_ERROR("kpanic() invoked");
    intr_disable();
    while (1) {
        cpu_halt();
    }
}