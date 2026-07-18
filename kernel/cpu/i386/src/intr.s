.section .text

.global intr_enable
.type intr_enable, @function
intr_enable:
    pushl %ebp
    movl %esp, %ebp
    
    sti
    
    popl %ebp
    ret

.global intr_disable
.type intr_disable, @function
intr_disable:
    pushl %ebp
    movl %esp, %ebp
    
    cli
    
    popl %ebp
    ret

.global intr_enabled
.type intr_enabled, @function
intr_enabled:
    pushl %ebp
    movl %esp, %ebp
    
    pushf
    popl %eax # transfer EFLAGS to EAX
    
    test $(1 << 9), %eax
    mov $0, %eax
    
    jz .done
    inc %eax # EAX=1 to indicate interrupt is enabled

.done:    
    popl %ebp
    ret