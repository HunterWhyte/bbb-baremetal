/* Copyright (c) 2023  Hunter Whyte */
#include <common.h>
#include <control.h>
#include <emif.h>
#include <gpio.h>
#include <interrupt.h>
#include <mmc.h>
#include <prcm.h>
#include <timer.h>
#include <uart.h>

/* external memory mapped to EMIF0 SDRAM starting at 0x80000000 */
#define DDR_START 0x80000000

/* MPU PLL Configuration based on AM335x TRM 8.1.6.9.1 */
/* 1GHz clock based on AM335x datasheet table 3.1 AM3358BZCZ100 */
void mpu_pll_init(void) {
  u32_t x;
  /* Switch PLL to bypass mode */
  x = REG(CM_CLKMODE_DPLL_MPU);
  x &= ~0x7;
  x |= 0x4;
  REG(CM_CLKMODE_DPLL_MPU) = x;
  /* wait for bypass status */
  while (!(REG(CM_IDLEST_DPLL_MPU) & 0x100)) {}

  /* configure divider and multipler */
  /* DPLL_MULT = 1000, DPLL_DIV = 23 (actual division factor is N+1) */
  /* 24MHz*1000/24 = 1GHz */
  REG(CM_CLKSEL_DPLL_MPU) = (1000 << 8) | (23);

  /* Set M2 Divider */
  REG(CM_DIV_M2_DPLL_MPU) &= ~0x1F;
  REG(CM_DIV_M2_DPLL_MPU) |= 1;

  /* Enable, locking PLL */
  x = REG(CM_CLKMODE_DPLL_MPU);
  x |= 0x7;
  REG(CM_CLKMODE_DPLL_MPU) = x;
  /* wait for locking to finish */
  while (!(REG(CM_IDLEST_DPLL_MPU) & 0x1)) {}
}

/* Core PLL Configuration based on AM335x TRM 8.1.6.7.1 */
/* All values based on AM335x TRM Table 8-22 Core PLL Typical Frequencies OPP100 */
/* clock source is 24MHz crystal on OSC0-IN (BBB schematic page 3) */
void core_pll_init(void) {
  u32_t x;
  /* Switch PLL to bypass mode */
  x = REG(CM_CLKMODE_DPLL_CORE);
  x &= ~0x7;
  x |= 0x4;
  REG(CM_CLKMODE_DPLL_CORE) = x;
  /* wait for bypass status */
  while (!(REG(CM_IDLEST_DPLL_CORE) & 0x100)) {}

  /* configure divider and multipler */
  /* DPLL_MULT = 1000, DPLL_DIV = 23 (actual division factor is N+1) */
  /* 24MHz*1000/24 = 1GHz */
  REG(CM_CLKSEL_DPLL_CORE) = (500 << 8) | (23);

  /* Set M4 Divider */
  REG(CM_DIV_M4_DPLL_CORE) &= ~0x1F;
  REG(CM_DIV_M4_DPLL_CORE) |= 10;

  /* Set the M5 Divider */
  REG(CM_DIV_M5_DPLL_CORE) &= ~0x1F;
  REG(CM_DIV_M5_DPLL_CORE) |= 8;

  /* Set the M6 Divider */
  REG(CM_DIV_M6_DPLL_CORE) &= ~0x1F;
  REG(CM_DIV_M6_DPLL_CORE) |= 4;

  /* Enable, locking PLL */
  x = REG(CM_CLKMODE_DPLL_CORE);
  x |= 0x7;
  REG(CM_CLKMODE_DPLL_CORE) = x;
  /* wait for locking to finish */
  while (!(REG(CM_IDLEST_DPLL_CORE) & 0x1)) {}
}

/* PER PLL Configuration based on AM335x TRM 8.1.6.8.1 */
/* All values based on AM335x TRM Table 8-24 PER PLL Typical Frequencies OPP100 */
/* clock source is 24MHz crystal on OSC0-IN (BBB schematic page 3) */
void per_pll_init(void) {
  u32_t x;
  /* Switch PLL to bypass mode */
  x = REG(CM_CLKMODE_DPLL_PER);
  x &= ~0x7;
  x |= 0x4;
  REG(CM_CLKMODE_DPLL_PER) = x;
  /* wait for bypass status */
  while (!(REG(CM_IDLEST_DPLL_PER) & 0x100)) {}

  /* configure divider and multipler */
  /* DPLL_MULT = 960, DPLL_DIV = 23 (actual division factor is N+1) */
  /* 24MHz*960/24 = 960MHz */
  REG(CM_CLKSEL_DPLL_PER) = (960 << 8) | (23);

  /* Set M2 Divider */
  REG(CM_DIV_M2_DPLL_PER) &= ~0x7F;
  REG(CM_DIV_M2_DPLL_PER) |= 5;

  /* Enable, locking PLL */
  x = REG(CM_CLKMODE_DPLL_PER);
  x |= 0x7;
  REG(CM_CLKMODE_DPLL_PER) = x;
  /* wait for locking to finish */
  while (!(REG(CM_IDLEST_DPLL_PER) & 0x1)) {}
}

/* DDR PLL Configuration based on AM335x TRM 8.1.6.11.1 */
/* 400MHz based on Table 5-5 of AM335x datasheet DDR3L max frequency */
/* clock source is 24MHz crystal on OSC0-IN (BBB schematic page 3) */
void ddr_pll_init(void) {
  u32_t x;
  /* Switch PLL to bypass mode */
  x = REG(CM_CLKMODE_DPLL_DDR);
  x &= ~0x7;
  x |= 0x4;
  REG(CM_CLKMODE_DPLL_DDR) = x;
  /* wait for bypass status */
  while (!(REG(CM_IDLEST_DPLL_DDR) & 0x100)) {}

  /* configure divider and multipler */
  /* DPLL_MULT = 400, DPLL_DIV = 23 (actual division factor is N+1) */
  /* 24MHz*400/24 = 400MHz */
  REG(CM_CLKSEL_DPLL_DDR) = (400 << 8) | (23);

  /* Set M2 Divider */
  REG(CM_DIV_M2_DPLL_DDR) &= ~0x1F;
  REG(CM_DIV_M2_DPLL_DDR) |= 1;

  /* Enable, locking PLL */
  x = REG(CM_CLKMODE_DPLL_DDR);
  x |= 0x7;
  REG(CM_CLKMODE_DPLL_DDR) = x;
  /* wait for locking to finish */
  while (!(REG(CM_IDLEST_DPLL_DDR) & 0x1)) {}
}

/* initialize all the interface clocks and prcm domains we will be using */
void interface_clocks_init(void) {
  /* WKUP domain enable */
  REG(CM_WKUP_CONTROL_CLKCTRL) = 0x2;
  /* PER domain enable */
  REG(CM_PER_L4LS_CLKCTRL) = 0x2;
  /* L3 interconnect clocks enable */
  REG(CM_PER_L3_CLKCTRL) = 0x2;

  /* WKUP domain force wakeup */
  REG(CM_WKUP_CLKSTCTRL) = 0x2;
  /* PER domain force wakeup */
  REG(CM_PER_L4LS_CLKSTCTRL) = 0x2;
  /* L3 interconnect force wakeup */
  REG(CM_PER_L3_CLKSTCTRL) = 0x2;
}

void ddr_init(void) {
  /* enable functional clock PD_PER_EMIF_GCLK */
  REG(CM_PER_EMIF_CLKCTRL) = 0x2;
  REG(CM_PER_EMIF_FW_CLKCTRL) = 0x2;

  /* wait for clocks to be enabled */
  while (!((REG(CM_PER_L3_CLKSTCTRL) & 0x4) && (REG(CM_PER_L3_CLKSTCTRL) & 0x8))) {}

  /* Note beaglebone black does not have VTT termination */
  /* initialize virtual temperature process compensation */
  REG(CONTROL_MODULE_VTP_CTRL) |= 0x40;
  REG(CONTROL_MODULE_VTP_CTRL) &= ~0x1;
  REG(CONTROL_MODULE_VTP_CTRL) |= 0x1;

  while (!(REG(CONTROL_MODULE_VTP_CTRL) & 0x20)) {}

  /* PHY CONFIG CMD */
  REG(CMD0_REG_PHY_CTRL_SLAVE_RATIO_0) = DDR3_CMD_SLAVE_RATIO;
  REG(CMD0_REG_PHY_INVERT_CLKOUT_0) = DDR3_CMD_INVERT_CLKOUT;

  REG(CMD1_REG_PHY_CTRL_SLAVE_RATIO_0) = DDR3_CMD_SLAVE_RATIO;
  REG(CMD1_REG_PHY_INVERT_CLKOUT_0) = DDR3_CMD_INVERT_CLKOUT;

  REG(CMD2_REG_PHY_CTRL_SLAVE_RATIO_0) = DDR3_CMD_SLAVE_RATIO;
  REG(CMD2_REG_PHY_INVERT_CLKOUT_0) = DDR3_CMD_INVERT_CLKOUT;

  /* PHY CONFIG DATA */
  REG(DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_0) = DDR3_DATA0_RD_DQS_SLAVE_RATIO;
  REG(DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_0) = DDR3_DATA0_WR_DQS_SLAVE_RATIO;
  REG(DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_0) = DDR3_DATA0_FIFO_WE_SLAVE_RATIO;
  REG(DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_0) = DDR3_DATA0_WR_DATA_SLAVE_RATIO;

  REG(DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_0) = DDR3_DATA0_RD_DQS_SLAVE_RATIO;
  REG(DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_0) = DDR3_DATA0_WR_DQS_SLAVE_RATIO;
  REG(DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_0) = DDR3_DATA0_FIFO_WE_SLAVE_RATIO;
  REG(DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_0) = DDR3_DATA0_WR_DATA_SLAVE_RATIO;

  /* IO CONTROL REGISTERS */
  REG(CONTROL_MODULE_DDR_CMD0_IOCTRL) = DDR3_IOCTRL_VALUE;
  REG(CONTROL_MODULE_DDR_CMD1_IOCTRL) = DDR3_IOCTRL_VALUE;
  REG(CONTROL_MODULE_DDR_CMD2_IOCTRL) = DDR3_IOCTRL_VALUE;
  REG(CONTROL_MODULE_DDR_DATA0_IOCTRL) = DDR3_IOCTRL_VALUE;
  REG(CONTROL_MODULE_DDR_DATA1_IOCTRL) = DDR3_IOCTRL_VALUE;

  /* IO to work for DDR3 */
  REG(CONTROL_MODULE_DDR_IO_CTRL) &= ~0x10000000;
  REG(CONTROL_MODULE_DDR_CKE_CTRL) |= 0x1;

  /* EMIF TIMING CONFIG */
  REG(EMIF0_DDR_PHY_CTRL_1) = DDR3_READ_LATENCY;
  REG(EMIF0_DDR_PHY_CTRL_1_SHDW) = DDR3_READ_LATENCY;
  REG(EMIF0_DDR_PHY_CTRL_2) = DDR3_READ_LATENCY;

  REG(EMIF0_SDRAM_TIM_1) = DDR3_SDRAM_TIMING1;
  REG(EMIF0_SDRAM_TIM_1_SHDW) = DDR3_SDRAM_TIMING1;

  REG(EMIF0_SDRAM_TIM_2) = DDR3_SDRAM_TIMING2;
  REG(EMIF0_SDRAM_TIM_2_SHDW) = DDR3_SDRAM_TIMING2;

  REG(EMIF0_SDRAM_TIM_3) = DDR3_SDRAM_TIMING3;
  REG(EMIF0_SDRAM_TIM_3_SHDW) = DDR3_SDRAM_TIMING3;

  REG(EMIF0_SDRAM_REF_CTRL) = DDR3_REF_CTRL;
  REG(EMIF0_SDRAM_REF_CTRL_SHDW) = DDR3_REF_CTRL;
  REG(EMIF0_ZQ_CONFIG) = DDR3_ZQ_CONFIG;
  REG(EMIF0_SDRAM_CONFIG) = DDR3_SDRAM_CONFIG;
}

/* read and write to some addresses in DDR, returns 0 on sucess */
u8_t ddr_check(void) {
  u32_t i;
  /* write to a bunch of addresses */
  for (i = 0; i < 0x20000000; i += 0x2000) {
    REG(DDR_START + i) = i;
  }
  /* read from the same addresses and compare with expected value */
  for (i = 0; i < 0x20000000; i += 0x2000) {
    if (REG(DDR_START + i) != i) {
      return 1;
    }
  }
  return 0;
}

void input_callback(char c) {
  /* echo input back out */
  uart_putc(c);
}

void timer_callback(void) {
  /* toggle LED gpio */
  gpio_led_toggle(2);
}

int main(void) {
  u32_t i, j;
  u32_t buf[128];
  u32_t kernel_start, kernel_size, kernel_writer;

  mpu_pll_init();
  core_pll_init();
  per_pll_init();
  ddr_pll_init();
  interface_clocks_init();

  REG(INTC_SYSCONFIG) |= (0x2);           /* trigger reset of INTC */
  while (!(REG(INTC_SYSSTATUS) & 0x1)) {} /* wait until INTC is reset.*/

  /* enable interrupts */
  irq_enable();

  gpio_led_init();
  uart_init(input_callback);
  timer_init(timer_callback);

  gpio_led_on(0);
  uart_puts("\r\n\r\bootloader started\r\n");
  uart_puts("UART initialized\r\n");

  uart_hexdump(0x01234567);
  uart_puts("\n\r");
  uart_hexdump(0x89ABCDEF);
  uart_puts("\n\r");

  /* initialize DDR3L, values hardcoded for D2516EC4BXGGB */
  ddr_init();
  if (REG(EMIF0_STATUS) & 0x4) {
    uart_puts("DDR3L initialized\n\r");
  } else {
    uart_puts("DDR3L initialization failed...\n\r");
    return 0;
  }
  /* check reading and writing to external DRAM before continuing */
  if (!ddr_check()) {
    uart_puts("DDR3L read/write check passed\n\r");
  } else {
    uart_puts("DDR3L read/write check failed...\n\r");
    return 0;
  }

  if (!mmc_init()) {
    uart_puts("MMC controller initialized\n\r");
  } else {
    uart_puts("MMC controller initialization failed...\n\r");
  }

  /* get length of bootloader section from header */
  if (mmc_read_block(buf, 1)) {
    return 0;
  }
  /* using GP header from MLO, add 512 to account for mandatory first sector */
  kernel_start = (buf[0] + 512);
  uart_puts("bootloader size: ");
  uart_hexdump(kernel_start);
  uart_puts("\n\r");

  /* offset rounds up to nearest block */
  kernel_start = (kernel_start / 512) + 1;
  uart_puts("kernel block: ");
  uart_hexdump(kernel_start);
  uart_puts("\n\r");

  if (mmc_read_block(buf, kernel_start)) {
    return 0;
  }
  kernel_size = buf[0];
  uart_puts("kernel size: ");
  uart_hexdump(kernel_size);
  uart_puts("\n\r");

  /* start from end of bootloader blocks and copy into external memory */
  /* first block is special case due to header */
  kernel_writer = DDR_START;
  for (i = 2; i < 128; i++) {
    REG(kernel_writer) = buf[i];
    kernel_writer += 4;
  }

  uart_puts("copying kernel...");
  for (i = 1; i < ((kernel_size / 512) + 1); i++) {
    if (mmc_read_block(buf, kernel_start + i)) {
      return 0;
    }
    for (j = 0; j < 128; j++) {
      REG(kernel_writer) = buf[j];
      kernel_writer += 4;
    }
    uart_puts(".");
  }

  uart_puts("\n\rstarting kernel\n\r\n\r\n\r");
  /* jump to kernel */
  asm(" ldr	r3, =0x80000000\n\t"
      " blx	r3\n\t");

  i = 0;
  /* infinite loop toggling LEDs */
  while (1) {
    i++;
    if (i == 50000) {
      gpio_led_toggle(1);
    }
    if (i == 100000) {
      gpio_led_toggle(1);
      i = 0;
    }
  }

  return 0;
}
