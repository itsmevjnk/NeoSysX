#ifndef __KERNEL_LOG_H
#define __KERNEL_LOG_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <timing/timer.h>

// TODO: implement proper kernel logging (kmsg style); these are just macros to append tags
#define LOG_INFO(fmt, ...) do { \
    uint64_t __ts = timer_get(); \
    printf("\x1B[102;30mI\x1B[49;92m %llu.%06u\x1B[0m " fmt "\n", __ts / TIMER_TICK_1_SEC, (unsigned)(__ts % TIMER_TICK_1_SEC) / 1000, ##__VA_ARGS__); \
} while(0)

#define LOG_WARN(fmt, ...) do { \
    uint64_t __ts = timer_get(); \
    printf("\x1B[103;30mW\x1B[49;93m %llu.%06u\x1B[0m " fmt "\n", __ts / TIMER_TICK_1_SEC, (unsigned)(__ts % TIMER_TICK_1_SEC) / 1000, ##__VA_ARGS__); \
} while(0)

#define LOG_ERROR(fmt, ...) do { \
    uint64_t __ts = timer_get(); \
    printf("\x1B[101;30mE\x1B[49;91m %llu.%06u\x1B[0m " fmt "\n", __ts / TIMER_TICK_1_SEC, (unsigned)(__ts % TIMER_TICK_1_SEC) / 1000, ##__VA_ARGS__); \
} while(0)

#define LOG_DEBUG(fmt, ...) do { \
    uint64_t __ts = timer_get(); \
    printf("\x1B[105;30mD\x1B[49;95m %llu.%06u\x1B[0m " fmt "\n", __ts / TIMER_TICK_1_SEC, (unsigned)(__ts % TIMER_TICK_1_SEC) / 1000, ##__VA_ARGS__); \
} while(0)

void ktrace(void); // print stack trace - to be implemented by CPU BSP
void ktrace_from(const void* frame_addr); // print stack trace from the specified frame - to be implemented by CPU BSP

#endif
