#ifndef __DRIVERS_TIMER_I8253_H
#define __DRIVERS_TIMER_I8253_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

extern const size_t pit_clock_freq; // input clock frequency in Hz (set with PIT_CLOCK_FREQ)

float pit_get_tick_freq(void);
float pit_init(float desired_freq);
void pit_disable(void);

#endif /* __DRIVERS_TIMER_I8253_H */