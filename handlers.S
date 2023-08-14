@ Copyright (c) 2023  Hunter Whyte
    .global vectors

    .text
	.arm

@ Vector table ARMv7 ARM Table B1-3
@ Bottom 5 bits of vector base address must be 0 (Cortex-A8 TRM 3.2.68)
.balign 32 /* byte align to 32-byte boundary*/
vectors:
    b entry                     @ Reset
    b undef_handler             @ Undefined Instruction
    b svc_handler               @ Supervisor Call
    b prefetch_abort_handler    @ Prefetch Abort
    b data_abort_handler        @ Data Abort
    b .                         @ Not Used
    b irq_handler               @ IRQ Interrupt
    b fiq_handler               @ FIQ Interrupt


undef_handler:
    b	.

svc_handler:
    b	.

prefetch_abort_handler:
    b	.

data_abort_handler:
    b	.

irq_handler:
    @ TODO: real IRQ handler
    stmfd sp!, {r0-r3, r12, lr}
    mrs r12, spsr 

    @ get the active IRQ number
    LDR      r1, =0x48200040
    LDR      r2, [r1]
    @ jump to isr
    ldr      r0, =isr_vectors
    add      r14, pc, #0
    ldr      pc, [r0, r2, lsl #2]

    @ Acknowledge interrupt and enable new IRQ generation
    mov r0, #0x1
    ldr r1, =0x48200048 @ INTC_CONTROL
    str r0, [r1]

    msr spsr, r12
    ldmfd sp!, {r0-r3, r12, lr}
    subs pc, lr, #4

fiq_handler:
	b	.
