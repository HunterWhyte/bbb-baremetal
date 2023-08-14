/* Copyright (c) 2023  Hunter Whyte */

#include <common.h>
#include <gpio.h>
#include <interrupt.h>
#include <prcm.h>
#include <timer.h>

static void (*timer_callback)(void) = NULL;

/* initialize TIMER0 peripheral and set it for periodic interrupts */
void timer_init(void (*callback)(void)) {

  REG(CM_WKUP_CONTROL_CLKCTRL) = 0x2;

  while (REG(CM_WKUP_TIMER0_CLKCTRL) & (0x3 << 16)) {}

  /* set auto-reload value and reset the counter*/
  /* The auto reload value controls the interrupt period.  [AM335x TRM 4447]
     period = (0xFFFFFFFF-TLDR + 1)/32kHz */
  REG(TIMER0_TLDR) = 0xFFFF8000;
  while (REG(TIMER0_TWPS) & (0x4)) {}

  REG(TIMER0_TTGR) = 0x1;
  while (REG(TIMER0_TWPS) & 0x8) {}

  /* set auto-reload */
  REG(TIMER0_TCLR) |= 0x2;
  while (REG(TIMER0_TWPS) & 0x1) {}
  gpio_led_on(0);

  /* set interrupts */
  REG(TIMER0_IRQENABLE_SET) |= 0x2;

  REG(TIMER0_TCLR) |= 0x1;
  while (REG(TIMER0_TWPS) & 0x1) {}

  timer_callback = callback;

  /* register timer ISR */
  irq_register(66, timer_isr);

  /* unmask TIMER0 interrupt 66 TIMER0 */
  REG(INTC_MIR_CLEAR2) = (0x1 << 2);
}

/* Interrupt service for TIMER 0 */
void timer_isr(void) {
  REG(TIMER0_IRQSTATUS) = 0x2;
  REG(INTC_ISR_CLEAR2) = (0x1 << 2);
  if (timer_callback != NULL) {
    timer_callback();
  }
  REG(INTC_CONTROL) = 0x1;
}
