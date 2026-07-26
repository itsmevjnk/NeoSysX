#include <tasking/thread.h>
#include <cpu/i386/idt.h>
#include <string.h>

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

void thread_init_kernel_stack(thread_tcb_t* thread, uintptr_t pc, uintptr_t user_sp) {
    size_t context_len = sizeof(idt_handler_context_t) - ((thread->attrib.user) ? 0 : (2 * 4)); // kernel threads do not have user ESP and user SS to restore
    idt_handler_context_t* context = (idt_handler_context_t*)(thread->kernel_sp - context_len);
    memset((void*)context, 0, context_len); // start out fresh
    context->eip = pc;
    context->eflags = (1 << 9) | (1 << 21); // enable interrupt and CPUID
    if (thread->attrib.user) {
        context->cs = 0x18 | 3;
        context->ds = context->es = context->fs = context->gs = context->user_ss = 0x20 | 3;
        context->user_esp = user_sp;
    } else {
        context->cs = 0x08;
        context->ds = context->es = context->fs = context->gs = 0x10;
    }
    // NOTE: we keep fsave_ptr null so that the FPU gets initialised on context switch

    thread->kernel_sp = (uintptr_t)context;
}
