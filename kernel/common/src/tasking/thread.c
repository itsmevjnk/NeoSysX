#include <tasking/thread.h>
#include <algo/idalloc.h>
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <timing/timer.h>
#include <stdlib.h>
#include <kernel/log.h>

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

static uint64_t thread_prev_yield_tick = 0; // tickstamp of last yield

void thread_yield(void* context) {
    thread_prev_yield_tick = timer_get();    
    thread_tcb_t* next_thread = (thread_current) ? thread_current->sched_next : NULL;
    thread_switch((next_thread) ? next_thread : &thread_kernel_tcb, context);
    // TODO
}

#ifndef THREAD_QUANTUM
#define THREAD_QUANTUM (1 * TIMER_TICK_1_MSEC)
#endif

void thread_evaluate_yield(void* context, void(*pre_yield_cb)(void* context, void* pre_yield_cb_param), void* pre_yield_cb_param) {
    if (!thread_current) return; // threading has not been initialised yet

    uint64_t elapsed_ticks = timer_get() - thread_prev_yield_tick;
    if (elapsed_ticks >= THREAD_QUANTUM) {
        if (pre_yield_cb) pre_yield_cb(context, pre_yield_cb_param);
        thread_yield(context);
    }
}

thread_pcb_t* thread_new_pcb(void) {
    thread_pcb_t* pcb = (thread_pcb_t*)malloc(sizeof(thread_pcb_t));
    if (!pcb) return NULL;

    pcb->id = idalloc_alloc(&thread_pid_allocator);
    if (pcb->id == SIZE_MAX) {
        LOG_ERROR("unable to allocate PID for new process");
        free((void*)pcb);
        return NULL;
    }

    pcb->vmm_config = vmm_new_config(NULL);
    if (!pcb->vmm_config) {
        LOG_ERROR("unable to create new VMM configuration for new process %lu", pcb->id);
        idalloc_free(&thread_pid_allocator, pcb->id);
        free((void*)pcb);
        return NULL;
    }

    pcb->next = thread_kernel_pcb.next;
    thread_kernel_pcb.next = pcb->next;
    pcb->thread_head = NULL;
    return pcb;
}

#ifndef THREAD_DEFAULT_USER_STACK_PAGES
#define THREAD_DEFAULT_USER_STACK_PAGES                     4
#endif

#ifndef THREAD_DEFAULT_KERNEL_STACK_PAGES
#define THREAD_DEFAULT_KERNEL_STACK_PAGES                   1
#endif

extern uintptr_t __user_stack_bottom; // to be defined in link.ld

thread_tcb_t* thread_new_tcb(thread_pcb_t* pcb, bool user, uintptr_t entry_pc) {
    thread_tcb_t* tcb = (thread_tcb_t*)malloc(sizeof(thread_tcb_t));

    tcb->id = idalloc_alloc(&thread_tid_allocator);
    if (tcb->id == SIZE_MAX) {
        LOG_ERROR("unable to allocate TID for new thread of PID %lu", pcb->id);
        free((void*)tcb);
        return NULL;
    }

    tcb->attrib.user = (user) ? 1 : 0;

    size_t frame_size = pmm_get_frame_size();
    const size_t kernel_stack_sz = frame_size * THREAD_DEFAULT_KERNEL_STACK_PAGES;
    uintptr_t kernel_sp = vmm_alloc_kernel_addrspace(kernel_stack_sz, VMM_FLAG_RW | VMM_FLAG_CACHE_GLOBAL);
    if (kernel_sp == UINTPTR_MAX) {
        LOG_ERROR("unable to allocate kernel stack for new thread with TID %lu (of PID %lu)", tcb->id, pcb->id);
        idalloc_free(&thread_tid_allocator, tcb->id);
        free((void*)tcb);
        return NULL;
    }
    tcb->kernel_sp = kernel_sp + kernel_stack_sz;

    uintptr_t user_sp = 0;
    if (user) {
        user_sp = vmm_alloc(pcb->vmm_config, frame_size * THREAD_DEFAULT_USER_STACK_PAGES, VMM_FLAG_RW | VMM_FLAG_USER, (uintptr_t)&__user_stack_bottom);
        if (user_sp == UINTPTR_MAX) {
            LOG_ERROR("unable to allocate user stack for new thread with TID %lu (of PID %lu)", tcb->id, pcb->id);
            vmm_free_pmm_frames_kernel(kernel_sp, kernel_stack_sz);
            idalloc_free(&thread_tid_allocator, tcb->id);
            free((void*)tcb);
            return NULL;
        }
    }

    tcb->proc = pcb;
    tcb->proc_next = pcb->thread_head;
    pcb->thread_head = tcb;

    tcb->sched_next = thread_kernel_tcb.sched_next; 
    thread_kernel_tcb.sched_next = tcb; // schedule after kernel thread

    thread_init_kernel_stack(tcb, entry_pc, user_sp);
    return tcb;
}
