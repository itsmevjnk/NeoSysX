#ifndef __TIMING_TIMER_H
#define __TIMING_TIMER_H

#include <stddef.h>
#include <stdint.h>

#define TIMER_TICK_1_SEC                                1000000000ULL
#define TIMER_TICK_1_MSEC                               1000000ULL
#define TIMER_TICK_1_USEC                               1000ULL
#define TIMER_TICK_1_NSEC                               1ULL

uint64_t timer_get(void); // get current system timer timestamp (in nanoseconds)
void timer_update(uint64_t delta); // increment system tick by the specified amount
void timer_reset(uint64_t tick); // reset system tick to the specified timestamp
void timer_stall(uint64_t duration); // stall for the specified number of nanoseconds

#endif /* __TIMING_TIMER_H */