#include <cpu/i386/idt.h>
#include <kernel/log.h>
#include <kernel/panic.h>

__attribute__((weak)) void exc_handler_stub(idt_handler_context_t* context, const char* mnemonic) {
    LOG_ERROR("unhandled exception %s (0x%02lx), error code 0x%08lx", mnemonic, context->vector, context->errcode);
    LOG_ERROR("EAX=0x%08lx EBX=0x%08lx ECX=0x%08lx EDX=0x%08lx", context->eax, context->ebx, context->ecx, context->edx);
    LOG_ERROR("ESI=0x%08lx EDI=0x%08lx ESP=0x%08lx EBP=0x%08lx", context->esi, context->edi, context->esp, context->ebp);
    LOG_ERROR("DS=0x%04lx ES=0x%04lx FS=0x%04lx GS=0x%04lx", context->ds, context->es, context->fs, context->gs);
    LOG_ERROR("CS:EIP=0x%04lx:0x%08lx EFLAGS=0x%08lx", context->cs, context->eip, context->eflags);
    if ((context->cs & 0b11) == 0b11) LOG_ERROR("user SS:ESP=0x%04lx:%08lx", context->user_ss, context->user_esp);

    LOG_DEBUG("stack trace:");
    ktrace_from((const void*)context->ebp);

    kpanic();
}

__attribute__((weak)) void exc_handler_DE(idt_handler_context_t* context) {
    exc_handler_stub(context, "DE");
}

__attribute__((weak)) void exc_handler_DB(idt_handler_context_t* context) {
    exc_handler_stub(context, "DB");
}

__attribute__((weak)) void exc_handler_BP(idt_handler_context_t* context) {
    exc_handler_stub(context, "BP");
}

__attribute__((weak)) void exc_handler_OF(idt_handler_context_t* context) {
    exc_handler_stub(context, "OF");
}

__attribute__((weak)) void exc_handler_BR(idt_handler_context_t* context) {
    exc_handler_stub(context, "BR");
}

__attribute__((weak)) void exc_handler_UD(idt_handler_context_t* context) {
    exc_handler_stub(context, "UD");
}

__attribute__((weak)) void exc_handler_NM(idt_handler_context_t* context) {
    exc_handler_stub(context, "NM");
}

__attribute__((weak)) void exc_handler_DF(idt_handler_context_t* context) {
    exc_handler_stub(context, "DF");
}

__attribute__((weak)) void exc_handler_CSO(idt_handler_context_t* context) {
    exc_handler_stub(context, "CSO");
}

__attribute__((weak)) void exc_handler_TS(idt_handler_context_t* context) {
    exc_handler_stub(context, "TS");
}

__attribute__((weak)) void exc_handler_NP(idt_handler_context_t* context) {
    exc_handler_stub(context, "NP");
}

__attribute__((weak)) void exc_handler_SS(idt_handler_context_t* context) {
    exc_handler_stub(context, "SS");
}

__attribute__((weak)) void exc_handler_GP(idt_handler_context_t* context) {
    exc_handler_stub(context, "GP");
}

__attribute__((weak)) void exc_handler_PF(idt_handler_context_t* context) {
    exc_handler_stub(context, "PF");
}

__attribute__((weak)) void exc_handler_MF(idt_handler_context_t* context) {
    exc_handler_stub(context, "MF");
}

__attribute__((weak)) void exc_handler_AC(idt_handler_context_t* context) {
    exc_handler_stub(context, "AC");
}

__attribute__((weak)) void exc_handler_MC(idt_handler_context_t* context) {
    exc_handler_stub(context, "MC");
}

__attribute__((weak)) void exc_handler_XM(idt_handler_context_t* context) {
    exc_handler_stub(context, "XM");
}

__attribute__((weak)) void exc_handler_VE(idt_handler_context_t* context) {
    exc_handler_stub(context, "VE");
}

__attribute__((weak)) void exc_handler_CP(idt_handler_context_t* context) {
    exc_handler_stub(context, "CP");
}

__attribute__((weak)) void exc_handler_reserved(idt_handler_context_t* context) {
    exc_handler_stub(context, "reserved");
}