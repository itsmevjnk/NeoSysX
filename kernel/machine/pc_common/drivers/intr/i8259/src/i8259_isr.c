#include <drivers/intr/i8259.h>
#include <cpu/i386/idt.h>
#include <kernel/log.h>

static pic_handler_node_t* pic_handler_heads[16] = { NULL };

void pic_register_handler(pic_handler_node_t* node) {
    if (node->irq > 15) return;
    node->next = pic_handler_heads[node->irq];
    pic_handler_heads[node->irq] = node;
}

void pic_unregister_handler(pic_handler_node_t* node) {
    if (node->irq > 15) return;
    if (!pic_handler_heads[node->irq]) return;

    if (pic_handler_heads[node->irq] == node) {
        pic_handler_heads[node->irq] = node->next;
    } else {
        pic_handler_node_t* prev_node = pic_handler_heads[node->irq];
        pic_handler_node_t* curr_node = pic_handler_heads[node->irq]->next;
        while (curr_node) {
            if (curr_node == node) {
                prev_node->next = curr_node->next;
                break;
            }
            curr_node = curr_node->next;
        }
    }
    node->next = NULL;
}

static size_t pic1_spurious_count = 0;
static size_t pic2_spurious_count = 0;

static void pic_handler_stub(uint8_t irq, idt_handler_context_t* context) {
    if (irq == 15 && !(pic2_get_isr() & (1 << 7))) { // PIC2 spurious interrupt
        pic2_spurious_count++;
        LOG_DEBUG("PIC2 spurious interrupt occurred (%lu so far)", pic2_spurious_count);
        pic_eoi(2); // still need to send EOI to PIC1
        return;
    }
    
    if (irq == 7 && !(pic1_get_isr() & (1 << 7))) { // PIC1 spurious interrupt
        pic1_spurious_count++;
        LOG_DEBUG("PIC1 spurious interrupt occurred (%lu so far)", pic1_spurious_count);
        return;
    }

    /* confirmed to not be a spurious interrupt */
    const pic_handler_node_t* node = pic_handler_heads[irq];
    if (!node) { // no handlers registered for interrupt
        LOG_WARN("no handlers have been registered for PIC IRQ %u", irq);
    } else {
        while (node) {
            // TODO: assert node->irq == irq
            node->handler(irq, context);
            node = node->next;
        }
    }
    pic_eoi(irq);
}

void pic_get_spurious_count(size_t* pic1, size_t* pic2) {
    if (pic1) *pic1 = pic1_spurious_count;
    if (pic2) *pic2 = pic2_spurious_count;
}

static void pic_handler_irq0(idt_handler_context_t* context) { pic_handler_stub(0, context); }
static void pic_handler_irq1(idt_handler_context_t* context) { pic_handler_stub(1, context); }
static void pic_handler_irq2(idt_handler_context_t* context) { pic_handler_stub(2, context); }
static void pic_handler_irq3(idt_handler_context_t* context) { pic_handler_stub(3, context); }
static void pic_handler_irq4(idt_handler_context_t* context) { pic_handler_stub(4, context); }
static void pic_handler_irq5(idt_handler_context_t* context) { pic_handler_stub(5, context); }
static void pic_handler_irq6(idt_handler_context_t* context) { pic_handler_stub(6, context); }
static void pic_handler_irq7(idt_handler_context_t* context) { pic_handler_stub(7, context); }
static void pic_handler_irq8(idt_handler_context_t* context) { pic_handler_stub(8, context); }
static void pic_handler_irq9(idt_handler_context_t* context) { pic_handler_stub(9, context); }
static void pic_handler_irq10(idt_handler_context_t* context) { pic_handler_stub(10, context); }
static void pic_handler_irq11(idt_handler_context_t* context) { pic_handler_stub(11, context); }
static void pic_handler_irq12(idt_handler_context_t* context) { pic_handler_stub(12, context); }
static void pic_handler_irq13(idt_handler_context_t* context) { pic_handler_stub(13, context); }
static void pic_handler_irq14(idt_handler_context_t* context) { pic_handler_stub(14, context); }
static void pic_handler_irq15(idt_handler_context_t* context) { pic_handler_stub(15, context); }

void pic_install_handlers(uint8_t pic1_offset, uint8_t pic2_offset) {
    idt_handlers[pic1_offset + 0] = pic_handler_irq0;
    idt_handlers[pic1_offset + 1] = pic_handler_irq1;
    idt_handlers[pic1_offset + 2] = pic_handler_irq2;
    idt_handlers[pic1_offset + 3] = pic_handler_irq3;
    idt_handlers[pic1_offset + 4] = pic_handler_irq4;
    idt_handlers[pic1_offset + 5] = pic_handler_irq5;
    idt_handlers[pic1_offset + 6] = pic_handler_irq6;
    idt_handlers[pic1_offset + 7] = pic_handler_irq7;
    idt_handlers[pic2_offset + 0] = pic_handler_irq8;
    idt_handlers[pic2_offset + 1] = pic_handler_irq9;
    idt_handlers[pic2_offset + 2] = pic_handler_irq10;
    idt_handlers[pic2_offset + 3] = pic_handler_irq11;
    idt_handlers[pic2_offset + 4] = pic_handler_irq12;
    idt_handlers[pic2_offset + 5] = pic_handler_irq13;
    idt_handlers[pic2_offset + 6] = pic_handler_irq14;
    idt_handlers[pic2_offset + 7] = pic_handler_irq15;
}