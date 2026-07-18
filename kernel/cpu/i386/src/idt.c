#include <cpu/i386/idt.h>

void idt_table_init(void); // implemented in idt_struct.c
extern idt_entry_t idt_table[256];
extern idt_descriptor_t idt_desc;

void idt_init(void) {
    idt_table_init();

    /* load IDTR */
    __asm__ volatile("lidt %0" : : "m"(idt_desc));
    // NOTE: enable interrupt with intr_enable when we're ready (i.e. after initialising/remapping PIC)
}

void idt_set_dpl(uint8_t vector, uint8_t dpl) {
    if (dpl > 3) return; // invalid DPL
    idt_table[vector].attrib.dpl = dpl;
}