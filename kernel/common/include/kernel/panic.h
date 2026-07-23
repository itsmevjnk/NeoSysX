#ifndef __KERNEL_PANIC_H
#define __KERNEL_PANIC_H

#include <stddef.h>
#include <stdint.h>

__attribute__((noreturn)) void kpanic(void); // halt and catch fire

#endif /* __KERNEL_PANIC_H */