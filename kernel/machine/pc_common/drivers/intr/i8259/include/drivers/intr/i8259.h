#ifndef __DRIVERS_INTR_I8259_H
#define __DRIVERS_INTR_I8259_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <cpu/i386/idt.h>

#define PIC1_DEFAULT_OFFSET             0x20
#define PIC2_DEFAULT_OFFSET             0x28

#define PIC_VECTOR(irq) (((irq) > 7) ? ((irq) - 8 + PIC2_DEFAULT_OFFSET) : ((irq) + PIC1_DEFAULT_OFFSET))

void pic_init(void); // initialise using default offsets
void pic_remap(uint8_t pic1_offset, uint8_t pic2_offset);
void pic_disable(void);
void pic_mask_irq(uint16_t mask);
void pic_unmask_irq(uint16_t mask);
void pic_eoi(uint8_t irq); // send EOI to the respective PIC
uint16_t pic_get_isr(void); // get the in-service register of both PICs
uint16_t pic_get_irr(void); // get the interrupt request register of both PICs
uint8_t pic1_get_isr(void); // get the in-service register of PIC1
uint8_t pic1_get_irr(void); // get the interrupt request register of PIC1
uint8_t pic2_get_isr(void); // get the in-service register of PIC2
uint8_t pic2_get_irr(void); // get the interrupt request register of PIC2

typedef void (*pic_handler_t)(uint8_t irq, idt_handler_context_t* context);
typedef struct pic_handler_node {
    pic_handler_t handler;
    struct pic_handler_node* next;
    uint8_t irq;
} pic_handler_node_t; // allows multiple handlers to be chained

void pic_install_handlers(uint8_t pic1_offset, uint8_t pic2_offset);
void pic_register_handler(pic_handler_node_t* node);
void pic_unregister_handler(pic_handler_node_t* node);
void pic_get_spurious_count(size_t* pic1, size_t* pic2);

#endif /* __DRIVERS_INTR_I8259_H */