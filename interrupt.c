/* Copyright (c) 2023  Hunter Whyte */
#include <common.h>
#include <interrupt.h>

u32_t isr_offset = 0x48200040;

#define NUM_INTERRUPTS (128u)
void (*isr_vectors[NUM_INTERRUPTS])(void);

/* Used when there is no isr defined for interrupt */
static void default_int_handler(void) {}

void irq_register(u32_t irq_number, void (*isr)(void)){
  if(irq_number < NUM_INTERRUPTS){
    isr_vectors[irq_number] = isr;
  }
}

/* Enable interrupts */
void irq_enable(void) {
  u32_t i;

  /* Register default handler for all interrupts */
  for (i = 0; i < NUM_INTERRUPTS; i++) {
    isr_vectors[i] = default_int_handler;
  }

  asm(" mrs r1, cpsr\n\t"
      " bic r1, #0x80\n\t"
      " msr cpsr_c, r1\n\t");
}