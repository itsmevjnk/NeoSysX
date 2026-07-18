#include <cpu/i386/idt.h>
#include <kernel/log.h>

idt_handler_t idt_handlers[256] = {
    exc_handler_DE,
    exc_handler_DB,
    NULL, // NMI
    exc_handler_BP,
    exc_handler_OF,
    exc_handler_BR,
    exc_handler_UD,
    exc_handler_NM,
    exc_handler_DF,
    exc_handler_CSO,
    exc_handler_TS,
    exc_handler_NP,
    exc_handler_SS,
    exc_handler_GP,
    exc_handler_PF,
    exc_handler_reserved,
    exc_handler_MF,
    exc_handler_AC,
    exc_handler_MC,
    exc_handler_XM,
    exc_handler_VE,
    exc_handler_CP,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
    exc_handler_reserved,
};

void idt_handler_stub(idt_handler_context_t* context) {
    if (idt_handlers[context->vector]) idt_handlers[context->vector](context);
    else LOG_WARN("unhandled interrupt 0x%02lx", context->vector);
}