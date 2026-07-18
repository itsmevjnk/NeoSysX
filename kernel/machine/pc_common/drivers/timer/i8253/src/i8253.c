#include <drivers/timer/i8253.h>
#include <kernel/log.h>
#include <timing/timer.h>
#include <cpu/x86_common/io.h>
#include <drivers/intr/i8259.h>

#ifndef PIT_CLOCK_FREQ
#define PIT_CLOCK_FREQ                              1193182
#endif

const size_t pit_clock_freq = PIT_CLOCK_FREQ;
static float pit_tick_freq = 0;

float pit_get_tick_freq(void) {
    return pit_tick_freq;
}

static float pit_tick_delta = 0; // to be initialised in pit_init
static float pit_tick_remainder = 0;

#define PIT_BASE                            0x40
#define PIT_CH0_DATA                        (PIT_BASE + 0)
#define PIT_CH1_DATA                        (PIT_BASE + 1)
#define PIT_CH2_DATA                        (PIT_BASE + 2)
#define PIT_COMMAND                         (PIT_BASE + 3)

/* PIT read/write modes */
#define PIT_RW_LSB                          1
#define PIT_RW_MSB                          2
#define PIT_RW_LSB_MSB                      3

#define PIT_SEND_COMMAND(ch, rw, mode, bcd) outb(PIT_COMMAND, ((ch) << 6) | ((rw) << 4) | ((mode) << 1) | (bcd))

static void pit_handler(uint8_t irq, idt_handler_context_t* context) {
    (void)irq; (void)context;
    
    pit_tick_remainder += pit_tick_delta;
    uint64_t delta = (uint64_t)pit_tick_remainder;
    pit_tick_remainder -= delta;
    timer_update(delta);
}

#define PIT_IRQ                             0
pic_handler_node_t pit_handler_node = {
    .handler = pit_handler,
    .next = NULL,
    .irq = PIT_IRQ
};

float pit_init(float desired_freq) {
    float divisor = PIT_CLOCK_FREQ / desired_freq;
    uint32_t divisor_int = (uint32_t)divisor;
    if (divisor - divisor_int >= 0.5f) divisor_int++;
    if (!divisor_int) divisor_int++;
    else if (divisor_int > 0xFFFF) divisor_int = 0xFFFF;
    pit_tick_freq = PIT_CLOCK_FREQ / (float)divisor_int;
    pit_tick_delta = TIMER_TICK_1_SEC / pit_tick_freq;

    LOG_INFO("using PIT as tick source with rate %.2f Hz (%.3f%% err)", pit_tick_freq, ((pit_tick_freq - desired_freq) / desired_freq) * 100.0f);

    PIT_SEND_COMMAND(0, PIT_RW_LSB_MSB, 3, 0); // set channel 0 to mode 3
    outb(PIT_CH0_DATA, (uint8_t)(divisor_int & 0xFF));
    outb(PIT_CH0_DATA, (uint8_t)((divisor_int >> 8) & 0xFF));

    pic_register_handler(&pit_handler_node);
    pic_unmask_irq(1 << PIT_IRQ);

    return pit_tick_freq;
}

void pit_disable(void) {
    pic_mask_irq(1 << PIT_IRQ);
}
