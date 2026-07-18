#ifndef __CPU_I386_IDT_H
#define __CPU_I386_IDT_H

#include <stddef.h>
#include <stdint.h>

#define IDT_GATE_TASK               0x5
#define IDT_GATE_INTR16             0x6
#define IDT_GATE_TRAP16             0x7
#define IDT_GATE_INTR32             0xE
#define IDT_GATE_TRAP32             0xF
typedef struct {
    uint16_t offset_lo : 16;
    uint16_t segment : 16;
    uint8_t reserved : 8;
    struct {
        uint8_t type : 4;
        uint8_t zero : 1;
        uint8_t dpl : 2;
        uint8_t present : 1;
    } __attribute__((packed)) attrib;
    uint16_t offset_hi : 16;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t size;
    idt_entry_t* offset;
} __attribute__((packed)) idt_descriptor_t;

#define IDT_ENTRY(seg, off, type_val, dpl_val) (idt_entry_t){ \
    .offset_lo = ((off) & 0xFFFF), \
    .segment = (seg), \
    .reserved = 0, \
    .attrib = { \
        .type = (type_val), \
        .zero = 0, \
        .dpl = (dpl_val), \
        .present = 1 \
    }, \
    .offset_hi = (((off) >> 16) & 0xFFFF) \
}
#define IDT_TASK_ENTRY(seg, off, dpl_val) IDT_ENTRY(seg, off, IDT_GATE_TASK, dpl_val)
#define IDT_INTR16_ENTRY(seg, off, dpl_val) IDT_ENTRY(seg, off, IDT_GATE_INTR16, dpl_val)
#define IDT_TRAP16_ENTRY(seg, off, dpl_val) IDT_ENTRY(seg, off, IDT_GATE_TRAP16, dpl_val)
#define IDT_INTR32_ENTRY(seg, off, dpl_val) IDT_ENTRY(seg, off, IDT_GATE_INTR32, dpl_val)
#define IDT_TRAP32_ENTRY(seg, off, dpl_val) IDT_ENTRY(seg, off, IDT_GATE_TRAP32, dpl_val)

typedef struct {
    // pushed by idt_handler_stub
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    // pushed by idt_handler_(intnum)
    uint32_t vector;
    uint32_t errcode; // only valid in exceptions with error code
    // pushed by the CPU itself
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    // only available if entering from ring 3 (CS[0:1] = 0b11)
    uint32_t user_esp;
    uint32_t user_ss;
} __attribute__((packed)) idt_handler_context_t;

typedef void (*idt_handler_t)(idt_handler_context_t* context);
extern idt_handler_t idt_handlers[256];

void idt_init(void);
void idt_set_dpl(uint8_t vector, uint8_t dpl); // by default all gates are ring 0 only - this allows a vector to be accessible to userland e.g. for syscalls

/* exception handlers - to be overridden */
void exc_handler_DE(idt_handler_context_t* context);
void exc_handler_DB(idt_handler_context_t* context);
void exc_handler_BP(idt_handler_context_t* context);
void exc_handler_OF(idt_handler_context_t* context);
void exc_handler_BR(idt_handler_context_t* context);
void exc_handler_UD(idt_handler_context_t* context);
void exc_handler_NM(idt_handler_context_t* context);
void exc_handler_DF(idt_handler_context_t* context);
void exc_handler_CSO(idt_handler_context_t* context);
void exc_handler_TS(idt_handler_context_t* context);
void exc_handler_NP(idt_handler_context_t* context);
void exc_handler_SS(idt_handler_context_t* context);
void exc_handler_GP(idt_handler_context_t* context);
void exc_handler_PF(idt_handler_context_t* context);
void exc_handler_MF(idt_handler_context_t* context);
void exc_handler_AC(idt_handler_context_t* context);
void exc_handler_MC(idt_handler_context_t* context);
void exc_handler_XM(idt_handler_context_t* context);
void exc_handler_VE(idt_handler_context_t* context);
void exc_handler_CP(idt_handler_context_t* context);
void exc_handler_reserved(idt_handler_context_t* context);

#endif /* __CPU_I386_IDT_H */