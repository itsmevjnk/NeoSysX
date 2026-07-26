#include <cpu/i386/init.h>

#include <cpu/i386/fpu.h>
#include <cpu/i386/gdt.h>
#include <cpu/i386/idt.h>

#include <kernel/log.h>

void kinit_cpu_pre(void) {
    fpu_init();
    gdt_init();
    idt_init();
}

void kinit_cpu_post(void) {
    switch (fpu_mode) {
        case 0: LOG_DEBUG("FPU is not supported"); break;
        case 1: LOG_DEBUG("FPU is supported without FXSAVE"); break;
        case 2: LOG_DEBUG("FPU is supported with FXSAVE"); break;
        default: LOG_ERROR("unknown fpu_mode value %lu", fpu_mode); break;
    }
}
