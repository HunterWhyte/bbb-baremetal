/* Copyright (c) 2023  Hunter Whyte */
/* Basic uart utilities for input from and output to UART on J1 header on
  Beaglebone Black. Could be expanded upon in the future to be more general.
*/
#include <common.h>
#include <gpio.h>
#include <interrupt.h>
#include <prcm.h>
#include <uart.h>

/* J1 Header on BBB board uses UART0_TX and UART0_RX */
/* hardware flow control (CTS/RTS) not connected */

void (*uart_callback)(char) = NULL;

/* initialize UART0 peripheral and set it up for polling I/O */
void uart_init(void (*callback)(char)) {
  /* Enable UART0 functional clock [AM335x TRM 1284] */
  REG(CM_WKUP_UART0_CLKCTRL) &= ~0x3; /* clear MODULEMODE */
  REG(CM_WKUP_UART0_CLKCTRL) |= 0x2;  /* MODULEMODE = enable */
  /* poll idle status waiting for fully enabled */
  while (REG(CM_WKUP_UART0_CLKCTRL) & (0x3 << 16)) {}

  /* Enable UART0 interface clock l4_wkup */
  REG(CM_WKUP_L4WKUP_CLKCTRL) &= ~0x3; /* clear MODULEMODE */
  REG(CM_WKUP_L4WKUP_CLKCTRL) |= 0x2;  /* MODULEMODE = enable */
  /* poll idle status waiting for fully enabled */
  while (REG(CM_WKUP_L4WKUP_CLKCTRL) & (0x3 << 16)) {}

  /* Control module pin muxing */
  REG(CONTROL_MODULE_UART0_RXD) = 0x30; /* pullup, receiver enabled */
  REG(CONTROL_MODULE_UART0_TXD) = 0x10; /* pullup */

  /* UART software reset */
  REG(UART0_SYSC) |= 0x2;               /* initiate software reset */
  while (!(REG(UART0_SYSS) & (0x1))) {} /* wait until end of reset operation */
  REG(UART0_SYSC) = 0x8;

  /* Disable UART to access protocol, baud rate, interrupt settings */
  REG(UART0_MDR1) |= 0x7;

  /* switch to register configuration mode A */
  REG(UART0_LCR) = 0x0080;
  /* disable modem control */
  REG(UART0_MCR) = 0x00;
  /* enable FIFO */
  REG(UART0_FCR) = 0x07;
  /* load divisor values to achieve baud rate of 115200 */
  /* [AM335x TRM table 19-25] for divisor values */
  REG(UART0_DLL) = 0x1A;
  REG(UART0_DLH) = 0x00;

  /* set protocol formatting */
  /* no parity, 1 stop bit, 8 bit chars */
  REG(UART0_LCR) = 0x000B;
  /* enable RHR interrupt */
  REG(UART0_IER_UART) = 0x1;
  /* mode select */
  REG(UART0_MDR1) = 0x0; /* clear all, UART 16x mode */

  REG(UART0_RESUME);

  uart_callback = callback;
  /* register UART ISR */
  irq_register(72, uart_isr);

  /* unmask UART0 interrupt */
  /* 72 UART0INT */
  /* each MIR register has 32 bits, so we are in MIR2. #72-(2*32) = 8 */
  REG(INTC_MIR_CLEAR2) = (0x1 << 8);
}

/* write char out to UART. blocks if Tx FIFO is full */
void uart_putc(char c) {
  while (!(REG(UART0_LSR_UART) & 0x40)) {}
  REG(UART0_THR) = c;
  gpio_led_toggle(3);
}

/* print null terminated string */
void uart_puts(char* c) {
  u32_t i = 0;
  while (c[i] != '\0') {
    uart_putc(c[i]);
    i++;
  }
}

static const char hexchars[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
/* utility to print out a 32 bit value */
void uart_hexdump(u32_t val) {
  s32_t i;
  uart_putc('0');
  uart_putc('x');
  /* print out all eight hex digits */
  for (i = 7; i >= 0; i--) {
    uart_putc(hexchars[(val >> (i * 4)) & 0xF]);
  }
}

/* poll for new character from UART, returns 0 if Rx FIFO is empty */
char uart_getc(void) {
  /* check if there is at least one character in Rx FIFO */
  if (!(REG(UART0_LSR_UART) & 0x1)) {
    /* TODO is 0 a valid char? return empty result in some other way? */
    return 0;
  } else {
    return REG(UART0_RHR) & 0xFF;
  }
}

/* TODO: assuming interrupt is RHR since its only one enabled */
void uart_isr(void) {
  char received;

  gpio_led_toggle(2);
  received = REG(UART0_RHR);
  if (uart_callback != NULL) {
    uart_callback(received);
  }
  REG(INTC_CONTROL) = 0x1;
}