/* Copyright (c) 2023  Hunter Whyte */
#ifndef _TIMER_H
#define _TIMER_H

void timer_init(void (*callback)(void));
void timer_isr(void);

#define TIDR_OFFSET 0x0
#define TIOCP_CFG_OFFSET 0x10
#define IRQ_EOI_OFFSET 0x20
#define IRQSTATUS_RAW_OFFSET 0x24
#define IRQSTATUS_OFFSET 0x28
#define IRQENABLE_SET_OFFSET 0x2c
#define IRQENABLE_CLEAR_OFFSET 0x30
#define IRQWAKEEN_OFFSET 0x34
#define TCLR_OFFSET 0x38
#define TCRR_OFFSET 0x3c
#define TLDR_OFFSET 0x40
#define TTGR_OFFSET 0x44
#define TWPS_OFFSET 0x48
#define TMAR_OFFSET 0x4c
#define TCAR1_OFFSET 0x50
#define TSICR_OFFSET 0x54
#define TCAR_OFFSET 0x58

#define DMTIMER0_BASE 0x44E05000
#define DMTIMER2_BASE 0x48040000
#define DMTIMER3_BASE 0x48042000
#define DMTIMER4_BASE 0x48044000
#define DMTIMER5_BASE 0x48046000
#define DMTIMER6_BASE 0x48048000
#define DMTIMER7_BASE 0x4804A000

#define TIMER0_TIDR (DMTIMER0_BASE + TIDR_OFFSET)
#define TIMER0_TIOCP (DMTIMER0_BASE + TIOCP_CFG_OFFSET)
#define TIMER0_IRQ_EOI (DMTIMER0_BASE + IRQ_EOI_OFFSET)
#define TIMER0_IRQSTATUS_RAW (DMTIMER0_BASE + IRQSTATUS_RAW_OFFSET)
#define TIMER0_IRQSTATUS (DMTIMER0_BASE + IRQSTATUS_OFFSET)
#define TIMER0_IRQENABLE_SET (DMTIMER0_BASE + IRQENABLE_SET_OFFSET)
#define TIMER0_IRQENABLE_CLEAR (DMTIMER0_BASE + IRQENABLE_CLEAR_OFFSET)
#define TIMER0_IRQWAKEEN (DMTIMER0_BASE + IRQWAKEEN_OFFSET)
#define TIMER0_TCLR (DMTIMER0_BASE + TCLR_OFFSET)
#define TIMER0_TCRR (DMTIMER0_BASE + TCRR_OFFSET)
#define TIMER0_TLDR (DMTIMER0_BASE + TLDR_OFFSET)
#define TIMER0_TTGR (DMTIMER0_BASE + TTGR_OFFSET)
#define TIMER0_TWPS (DMTIMER0_BASE + TWPS_OFFSET)
#define TIMER0_TMAR (DMTIMER0_BASE + TMAR_OFFSET)
#define TIMER0_TCAR1 (DMTIMER0_BASE + TCAR1_OFFSET)
#define TIMER0_TSICR (DMTIMER0_BASE + TSICR_OFFSET)
#define TIMER0_TCAR (DMTIMER0_BASE + TCAR_OFFSET)


#endif /* _TIMER_H */