#include <drivers/intr/i8259.h>
#include <cpu/x86_common/io.h>

#define PIC1_BASE                       0x20
#define PIC2_BASE                       0xA0
#define PIC1_COMMAND                    (PIC1_BASE + 0)
#define PIC1_DATA                       (PIC1_BASE + 1)
#define PIC2_COMMAND                    (PIC2_BASE + 0)
#define PIC2_DATA                       (PIC2_BASE + 1)

void pic_init(void) {
    pic_remap(PIC1_DEFAULT_OFFSET, PIC2_DEFAULT_OFFSET);
    pic_install_handlers(PIC1_DEFAULT_OFFSET, PIC2_DEFAULT_OFFSET);
}

#define PIC_ICW1_ICW4                   (1 << 0)
#define PIC_ICW1_SINGLE                 (1 << 1)
#define PIC_ICW1_INTERVAL4              (1 << 2)
#define PIC_ICW1_LEVEL                  (1 << 3)
#define PIC_ICW1_INIT                   (1 << 4)

#define PIC_ICW4_8086                   (1 << 0)
#define PIC_ICW4_AUTO                   (1 << 1)
#define PIC_ICW4_BUF_SLAVE              (1 << 3)
#define PIC_ICW4_BUF_MASTER             ((1 << 3) | (1 << 2))
#define PIC_ICW4_SFNM                   (1 << 4)

#define PIC_CASCADE_IRQL                2

void pic_remap(uint8_t pic1_offset, uint8_t pic2_offset) {
    outb(PIC1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4); io_wait();
    outb(PIC2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4); io_wait();

    outb(PIC1_DATA, pic1_offset); io_wait();
    outb(PIC2_DATA, pic2_offset); io_wait();

    outb(PIC1_DATA, (1 << 2)); io_wait();
    outb(PIC2_DATA, (1 << 1)); io_wait();
    
    outb(PIC1_DATA, PIC_ICW4_8086); io_wait();
    outb(PIC2_DATA, PIC_ICW4_8086); io_wait();

    /* mask all interrupts initially */
    outb(PIC1_DATA, ~(1 << PIC_CASCADE_IRQL));
    outb(PIC2_DATA, 0xFF);
}

void pic_disable(void) {
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}

void pic_mask_irq(uint16_t mask) {
    if (mask & 0x00FF) { // set PIC1
        outb(PIC1_DATA, (inb(PIC1_DATA) | (mask & 0xFF)) & ~(1 << PIC_CASCADE_IRQL)); // keep IRQ2 (cascade) unmasked for PIC2 to work
    }
    
    if (mask & 0xFF00) { // set PIC2
        outb(PIC2_DATA, inb(PIC2_DATA) | ((mask >> 8) & 0xFF));
    }
}

void pic_unmask_irq(uint16_t mask) {
    if (mask & 0x00FF) { // set PIC1
        outb(PIC1_DATA, inb(PIC1_DATA)& ~((1 << PIC_CASCADE_IRQL) | (mask & 0xFF))); // keep IRQ2 (cascade) unmasked for PIC2 to work
    }
    
    if (mask & 0xFF00) { // set PIC2
        outb(PIC2_DATA, inb(PIC2_DATA) & ~((mask >> 8) & 0xFF));
    }
}

#define PIC_EOI                         0x20
void pic_eoi(uint8_t irq) {
    if (irq > 7) outb(PIC2_COMMAND, PIC_EOI);
    outb(PIC1_COMMAND, PIC_EOI); // due to cascading we'll need to send EOI to the master PIC regardless
}

#define PIC_READ_ISR                    0x0B
#define PIC_READ_IRR                    0x0A

uint8_t pic1_get_isr(void) {
    outb(PIC1_COMMAND, PIC_READ_ISR);
    return inb(PIC1_COMMAND);
}

uint8_t pic1_get_irr(void) {
    outb(PIC1_COMMAND, PIC_READ_IRR);
    return inb(PIC1_COMMAND);
}

uint8_t pic2_get_isr(void) {
    outb(PIC2_COMMAND, PIC_READ_ISR);
    return inb(PIC2_COMMAND);
}

uint8_t pic2_get_irr(void) {
    outb(PIC2_COMMAND, PIC_READ_IRR);
    return inb(PIC2_COMMAND);
}

uint16_t pic_get_isr(void) {
    return ((uint16_t)pic2_get_isr() << 8) | pic1_get_isr();
}

uint16_t pic_get_irr(void) {
    return ((uint16_t)pic2_get_irr() << 8) | pic1_get_irr();
}