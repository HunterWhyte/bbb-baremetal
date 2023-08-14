/* Copyright (c) 2023  Hunter Whyte */
#ifndef _INTERRUPT_H
#define _INTERRUPT_H
#include <common.h>

void irq_enable(void);
void irq_isr(void);
void irq_register(u32_t irq_number, void (*isr)(void));

#define INTC_BASE 0x48200000
#define INTC_SYSCONFIG (INTC_BASE + 0x10)
#define INTC_SYSSTATUS (INTC_BASE + 0x14)
#define INTC_MIR_CLEAR2 (INTC_BASE + 0xC8)
#define INTC_CONTROL (INTC_BASE + 0x48)

#define INTC_ISR_CLEAR2 (INTC_BASE + 0xD4)
#define INTC_SIR_IRQ (INTC_BASE + 0x40)

#endif /* _INTERRUPT_H */
