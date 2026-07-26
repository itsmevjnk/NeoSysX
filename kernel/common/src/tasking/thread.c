#include <tasking/thread.h>
#include <algo/idalloc.h>
#include <mm/vmm.h>

thread_tcb_t thread_kernel_tcb = { .proc = &thread_kernel_pcb };
thread_pcb_t thread_kernel_pcb = { .thread_head = &thread_kernel_tcb };

#ifndef THREAD_INITIAL_MAX_PROCESSES
#define THREAD_INITIAL_MAX_PROCESSES 16384
#endif
idalloc_t thread_pid_allocator = { .size = THREAD_INITIAL_MAX_PROCESSES };

#ifndef THREAD_INITIAL_MAX_THREADS
#define THREAD_INITIAL_MAX_THREADS 16384
#endif
idalloc_t thread_tid_allocator = { .size = THREAD_INITIAL_MAX_THREADS };

thread_tcb_t* thread_current = NULL;

void thread_init(void) {
    idalloc_init(&thread_pid_allocator);
    idalloc_init(&thread_tid_allocator);

    thread_kernel_pcb.id = idalloc_alloc(&thread_pid_allocator);
    thread_kernel_pcb.vmm_config = vmm_get_kernel_config();

    thread_kernel_tcb.id = idalloc_alloc(&thread_tid_allocator);
    thread_init_state(); // load kernel SP and context, then perform context switch to land back here
    thread_current = &thread_kernel_tcb;
}

void thread_yield(void* context) {
    thread_tcb_t* next_thread = (thread_current) ? thread_current->sched_next : NULL;
    thread_switch((next_thread) ? next_thread : &thread_kernel_tcb, context);
    // TODO
}
