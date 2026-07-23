#include <timing/timer.h>
#include <cpu/halt.h>

static volatile uint64_t timer_ticks = 0;

uint64_t timer_get(void) {
    return timer_ticks;
}

void timer_update(uint64_t delta) {
    timer_ticks += delta;
}

void timer_reset(uint64_t tick) {
    timer_ticks = tick;
}

void timer_stall(uint64_t duration) {
    volatile uint64_t t_end = timer_ticks + duration;
    if (t_end < timer_ticks) { // rollover
        while (timer_ticks > 0) cpu_halt(); // wait until timer ticks go back to 0
    }
    while (timer_ticks < t_end) cpu_halt(); // wait until it reaches t_end
}