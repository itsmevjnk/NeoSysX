#ifndef __TASKING_THREAD_H
#define __TASKING_THREAD_H

#include <stddef.h>
#include <stdint.h>

/* process control block */
struct thread_pcb {
    uintptr_t id; // process ID
    void* vmm_config; // process' VMM config structure pointer (i.e. its address space)
    struct thread_tcb* thread_head; // first TCB within this process
} __attribute__((packed));

/* thread control block */
struct thread_tcb {
    uintptr_t id; // thread ID
    struct thread_pcb* proc; // parent process
    struct thread_tcb* proc_next; // next thread in the process linked list
    struct thread_tcb* sched_next; // next thread in the scheduling linked list
    uintptr_t kernel_sp; // thread's kernel stack pointer (user-space stack pointer is stored within the kernel stack)
    // NOTE: The kernel stack pointer should point to the thread's stack immediately before restoring context.
    //       This way we can execute an interrupt return-style context restore and land at the thread's state prior to
    //       the context switch.
    struct {
        uint8_t priority : 8;
        uint8_t user : 1; // whether this is an user thread
        uint32_t reserved : 23;
    } __attribute__((packed)) attrib;
} __attribute__((packed));

typedef struct thread_pcb thread_pcb_t;
typedef struct thread_tcb thread_tcb_t;

extern thread_pcb_t thread_kernel_pcb;
extern thread_tcb_t thread_kernel_tcb;

extern thread_tcb_t* thread_current; // currently running thread

void thread_init(void); // initialise threading by creating a kernel thread using the current state (performed by thread_init_state(); see below) and performing a context switch to it
// NOTE: sanity check for thread_init(): stack state and reserved registers must be similar with or without thread_init_state()
void thread_yield(void* context); // yield to the next thread; this is to be called in an ISR (which also stores context)

// to be implemented by CPU/target BSP
void thread_switch(thread_tcb_t* thread, void* context); // save context for the current thread, and then switch to the specified thread
// NOTE: context can be provided by the ISR that triggers the switch; see thread_yield()
void thread_init_state(void); // initialise the state for the kernel thread, then perform a context switch to return out of the function

#endif
