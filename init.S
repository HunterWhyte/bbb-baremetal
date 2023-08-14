@ Copyright (c) 2023  Hunter Whyte

	.global _bss_start
	.global _bss_end
	.global _stack_top
	.global entry

	.text
	.arm

entry:
	@ TODO: have to set up stack pointers for all the other modes
	@ set IRQ stack pointer
	@ switch to IRQ mode
	msr cpsr_c, #0xD2
	ldr	r0, =_stack_top
	mov sp, r0
	sub r0, r0, #0x400

	@ switch to system mode
	msr cpsr_c, #0xDF
    mov sp, r0 @ set stack pointer

@ clear BSS
bss_setup:
	ldr	r0, =_begin_bss
	ldr	r1, =_end_bss
	mov	r2, #0
loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	blt	loop

@ set vector base address (Cortex-A8 TRM 3.2.68)
	ldr r0,	=vectors
	mcr p15, #0, r0, c12, c0, #0

enter_main:
	ldr	r3, =main
	blx	r3
	b	enter_main



