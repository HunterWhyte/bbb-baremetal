/* Copyright (c) 2023  Hunter Whyte */
/* Basic gpio utilities to control LEDs on Beaglebone Black.
  Could be expanded upon in the future to support all 4 GPIO modules + much more
  general functionality than just setting and clearing LED pins specifically.
*/
#include <common.h>
#include <gpio.h>
#include <prcm.h>

/* enable GPIO1 to use user LEDs, doesn't enable GPIO 0, 2, or 3 */
void gpio_led_init(void) {
  /* GPIO1 clock and power config [AM335x TRM 1284] */
  REG(CM_PER_GPIO1_CLKCTRL) &= ~0x3;      /* clear MODULEMODE */
  REG(CM_PER_GPIO1_CLKCTRL) |= 0x2;       /* MODULEMODE = enable */
  REG(CM_PER_GPIO1_CLKCTRL) |= (1 << 18); /* enable optional debounce clock */
  /* poll idle status waiting for fully enabled */
  while (REG(CM_PER_GPIO1_CLKCTRL) & (0x3 << 16)) {}

  /* GPIO1 config [AM335x TRM 4990]*/
  REG(GPIO1_SYSCONFIG) |= (0x2);           /* trigger reset of GPIO1 */
  while (!(REG(GPIO1_SYSSTATUS) & 0x1)) {} /* wait until GPIO1 is reset.*/
  REG(GPIO1_CTRL) &= ~(0x1);               /* clear disable bit, enabling GPIO1 */

  /* set user LED pins to output mode */
  REG(GPIO1_OE) &= ~((1 << USR0) | (1 << USR1) | (1 << USR2) | (1 << USR3));
}

/* set a pin hooked up to LED on bbb, gpio_init must be called first */
void gpio_led_on(s32_t led_number) {
  if (led_number >= 0 && led_number < 4) {
    REG(GPIO1_SETDATAOUT) = (1 << (USR0 + led_number));
  }
}

/* clear a pin hooked up to LED on bbb, gpio_init must be called first */
void gpio_led_off(s32_t led_number) {
  if (led_number >= 0 && led_number < 4) {
    REG(GPIO1_CLEARDATAOUT) = (1 << (USR0 + led_number));
  }
}

/* toggle an LED on or off. gpio_init must be called first */
void gpio_led_toggle(s32_t led_number) {
  if (led_number >= 0 && led_number < 4) {
    if(REG(GPIO1_DATAOUT) & (1 << (USR0 + led_number))){
      REG(GPIO1_CLEARDATAOUT) = (1 << (USR0 + led_number));
    } else {
      REG(GPIO1_SETDATAOUT) = (1 << (USR0 + led_number));
    }
  }
}