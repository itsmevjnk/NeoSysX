.section .text
.align 16
.extern thread_current
.extern vmm_current_config
.extern idt_restore_ctx # in idt.s
.global thread_switch
.type thread_switch, @function
thread_switch:
    pushl %ebp
    movl %esp, %ebp

    cli # NO MORE INTERRUPTS HERE!!!!!!!!! (we will get interrupts back after IRET)

    movl 8(%ebp), %ecx # thread to switch to
    movl 12(%ebp), %edx # current thread's IDT context

    movl thread_current, %eax
    cmpl $0, %eax
    jz .first_save
.save_ctx: # save context for thread_current
    movl %edx, 28(%eax)
    jmp .save_done
.first_save: # special case - save context into kernel thread (which is also passed in ECX)
    movl %edx, 28(%ecx)
.save_done:
    movl %ecx, thread_current # update current thread

    # switch VMM config - this is essentially an alternative fully-ASM implementation of vmm_switch
    movl vmm_current_config, %edx
    movl 4(%ecx), %eax # thread->proc_node.list (aka &proc.thread_list)
    subl $(4 * (3 + 1)), %eax # &proc.vmm_config
    movl (%eax), %eax # proc.vmm_config
    cmpl %edx, %eax
    je .vmm_done # same configuration - avoid switching which will cause TLB flush
    movl 8192(%eax), %eax # proc.vmm_config->cr3
    movl %eax, %cr3
    # since all our data is in the synchronised kernel space, we should be able to survive this switch
.vmm_done:

    # restore context
    movl 28(%ecx), %esp # thread->kernel_sp
    # TODO: set ESP0 to end of idt_context_t struct for user threads

    jmp idt_restore_ctx # reuse IDT handler's restoration function for this
    # it will also do IRET for us