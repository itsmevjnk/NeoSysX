#include <kernel/main.h>
#include <kernel/log.h>
#include <cpu/halt.h>

#include <mm/pmm.h>
#include <mm/vmm.h>

#include <tasking/thread.h>

#include <liballoc.h>
#include <stdio.h>

/* placeholder */
__attribute__((weak)) void kinit_target_pre(void) {

}

__attribute__((weak)) void kinit_target_post(void) {

}

void test_thread1_entry(void) {
    while (1) {
        putchar('1');
    }
}

void test_thread2_entry(void) {
    while (1) {
        putchar('2');
    }
}

void kmain(void) {
    LOG_DEBUG("entering kinit_target_pre");
    kinit_target_pre();

    // TODO: initialise kernel
    pmm_init();
    vmm_init();
    liballoc_init();
    thread_init();
    
    LOG_DEBUG("entering kinit_target_post");
    kinit_target_post();

    LOG_INFO("kernel init complete");

    LOG_INFO("creating new thread");
    thread_new_tcb(&thread_kernel_pcb, false, (uintptr_t)&test_thread1_entry);
    thread_new_tcb(&thread_kernel_pcb, false, (uintptr_t)&test_thread2_entry);
    while (1) {
        putchar('K');
    }
}