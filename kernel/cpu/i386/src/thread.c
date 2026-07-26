#include <tasking/thread.h>
#include <cpu/i386/idt.h>

// initial switch handler
static void thread_init_intr_handler(idt_handler_context_t* context) {
    thread_kernel_tcb.kernel_sp = (uintptr_t)context; // our convention: kernel_sp points to the IDT context (so that it all gets flushed out during interrupt return)
    thread_switch(&thread_kernel_tcb, (void*)context);
}

void thread_init_state(void) {
    // we're using 0xF0 as our initial interrupt
    idt_handler_t old_handler = idt_handlers[0xF0];
    idt_handlers[0xF0] = thread_init_intr_handler;
    __asm__ volatile("int $0xF0" : : : "memory"); // DAVAI!
    // int 0xF0 should drop us back here, with all registers intact
    idt_handlers[0xF0] = old_handler;
}
