#include <kernel/log.h>

struct stackframe {
    struct stackframe* ebp;
    uint32_t eip;
};

extern uintptr_t __kernel_start;

void ktrace(void) {
    const void* frame_addr;
    __asm__ volatile("movl %%ebp, %0" : "=r"(frame_addr) ::);
    ktrace_from(frame_addr);
}

void ktrace_from(const void* frame_addr) {
    const struct stackframe* frame = (const struct stackframe*)frame_addr;
    for (int i = 0; (uintptr_t)frame >= (uintptr_t)&__kernel_start; i++, frame = frame->ebp) {
        LOG_DEBUG("stack frame %d [0x%lx]: EIP=0x%lx", i, (uintptr_t)frame, frame->eip);
    }
}
