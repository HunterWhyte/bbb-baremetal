/* Copyright (c) 2023  Hunter Whyte */
/* basic SD card initialization based on SD standard Physical Layer Simplified 
   Specification Version 9.00 [1] 
   TODO: currently only supports SD cards compliant with standard 2.0 or later.
   Recommended control flow for identifying SD card type is mostly skipped.
*/

#include <common.h>
#include <control.h>
#include <mmc.h>
#include <prcm.h>
#include <uart.h>

u32_t rca;

/* returns 0 on success */
int mmc_send_command(u32_t command, u32_t response_type, u32_t flags, u32_t arg) {
  REG(MMC0_SD_ARG) = arg;
  REG(MMC0_SD_CMD) = (command << 24) | (response_type << 16) | flags;
  /* wait for command complete or an error to be raised */
  while (!(REG(MMC0_SD_STAT))) {}
  /* check if an error was raised */
  if (REG(MMC0_SD_STAT) & (0x1 << 15)) {
    uart_puts("error on MMC command. SD_STAT: ");
    uart_hexdump(REG(MMC0_SD_STAT));
    uart_puts("\r\n");
    /* clear all status */
    REG(MMC0_SD_STAT) = 0xFFFFFFFF;
    return 1;
  }

  /* if its a busy type command, have to wait for transfer complete bit as well */
  if (response_type == MMC_RSP_48_BUSY) {
    while (!(REG(MMC0_SD_STAT) & 0x2)) {}
    /* clear TC status */
    REG(MMC0_SD_STAT) = 0x2;
  }

  /* clear CC status */
  REG(MMC0_SD_STAT) = 0x1;
  return 0;
}

/* blocking read data into buffer returns 0 on success */
int mmc_read_block(u32_t* buf, u32_t block) {
  u32_t i, timeout;

  /* set block size to 512 */
  REG(MMC0_SD_IE) |= (0x1 << 5);
  /* set block size to 512 */
  REG(MMC0_SD_BLK) = 0x200;

  /* | (0x1 << 20) | (0x1 << 19) */
  if (mmc_send_command(MMC_CMD17_READ_SINGLE_BLOCK, MMC_RSP_48, (0x1 << 21) | (0x1 << 4), block)) {
    return 1;
  }

  timeout = 0;
  /* poll waiting for buffer read ready event or error */
  while (!(REG(MMC0_SD_STAT) & ((0x1 << 5) | (0x1 << 15)))) {
    timeout++;
    if (timeout > 100000) {
      uart_puts("\r\ntimeout on MMC block read. SD_STAT: ");
      uart_hexdump(REG(MMC0_SD_STAT));
      uart_puts("\r\n");
      REG(MMC0_SD_STAT) = 0xFFFFFFFF;
      return 1;
    }
  }

  if (REG(MMC0_SD_STAT) & (0x1 << 15)) {
    uart_puts("\r\nerror on MMC block read. SD_STAT: ");
    uart_hexdump(REG(MMC0_SD_STAT));
    uart_puts("\r\n");
    REG(MMC0_SD_STAT) = 0xFFFFFFFF;
    return 1;
  }
  /* copy data into buffer */
  for (i = 0; i < 128; i++) {
    buf[i] = REG(MMC0_SD_DATA);
  }

  /* wait for TC or error */
  while (!(REG(MMC0_SD_STAT) & ((0x1 << 15) | (0x1 << 1)))) {
  }
  if (REG(MMC0_SD_STAT) & (0x1 << 15)) {
    /* error */
    uart_puts("error on MMC data transfer. SD_STAT: ");
    uart_hexdump(REG(MMC0_SD_STAT));
    uart_puts("\r\n");
    /* clear all status */
    REG(MMC0_SD_STAT) = 0xFFFFFFFF;
  }

  /* clear buffer read ready event */
  REG(MMC0_SD_STAT) = (0x1 << 5) | (0x1 << 1);
  return 0;
}

/* returns 0 on success */
/* initialize MMC0 module for SD card */
int mmc_init(void) {
  /* enable functional clock for mmc0 */
  REG(CM_PER_MMC0_CLKCTRL) |= 0x2;

  /* pinmuxing, uses data pins 0-3 */
  /* mmode 0, puden pullup/down enabled, typesel pullup selected, receiver enabled*/
  REG(CONTROL_MODULE_CONF_MMC0_DAT3) = 0x30;
  REG(CONTROL_MODULE_CONF_MMC0_DAT2) = 0x30;
  REG(CONTROL_MODULE_CONF_MMC0_DAT1) = 0x30;
  REG(CONTROL_MODULE_CONF_MMC0_DAT0) = 0x30;
  REG(CONTROL_MODULE_CONF_MMC0_CLK) = 0x30;
  REG(CONTROL_MODULE_CONF_MMC0_CMD) = 0x30;

  /* software reset of controller */
  REG(MMC0_SD_SYSCONFIG) |= (0x2);           /* trigger reset of MMC0 */
  while (!(REG(MMC0_SD_SYSSTATUS) & 0x1)) {} /* wait until MMC0 is reset.*/
  uart_puts("MMC0 clock and pinmuxing...");

  /* set 3.3V as supported voltage */
  REG(MMC0_SD_CAPA) |= (7 << 24);

  REG(MMC0_SD_SYSCONFIG) |= (0x1) | (0x1 << 2) | (0x2 << 3) | (0x2 << 12);
  /* intterupt wakeup enable */
  REG(MMC0_SD_HCTL) |= (1 << 24);

  /* Write SD_CON register DW8 to configure specific data and
    command transfer */
  /* DW8 1-bit transfer mode for initialization required */
  REG(MMC0_SD_CON) &= ~(0x1 << 5);

  /*Write SD_HCTL register (SDVS, SDBP, DTW) to configure the card voltage
    value and power mode and data bus width*/
  /* SDBP SD bus power off */
  REG(MMC0_SD_HCTL) &= ~(0x1 << 8);
  /* SDVS SD bus voltage select 3.3V */
  REG(MMC0_SD_HCTL) |= (0x6 << 9);
  /* DTW data transfer width, 1 bit */
  REG(MMC0_SD_HCTL) &= ~(0x1 << 1);

  /* SDBP SD bus power on */
  REG(MMC0_SD_HCTL) |= (0x1 << 8);
  while (!(REG(MMC0_SD_HCTL) & (0x1 << 8))) {
    uart_putc('.');
  }

  /* Enable internal clock */
  REG(MMC0_SD_SYSCTL) |= 0x1;
  /* Set the initialization frequency CLKD. 96MHz functional clock input */
  /* intialization clock speed is as slow as possible, 96MHz/1024 = ~93Khz */
  REG(MMC0_SD_SYSCTL) &= ~(0x3FF << 6);
  REG(MMC0_SD_SYSCTL) |= (0x240 << 6);
  /* external clock enable */
  REG(MMC0_SD_SYSCTL) |= (0x1 << 2);
  /* wait for internal clock to be stable */
  while (!(REG(MMC0_SD_SYSCTL) & 0x2)) {
    uart_putc('.');
  }
  uart_puts("MMC0 host control setup...");

  /* enable all the interrupt event flags */
  REG(MMC0_SD_IE) |= 0xFFFFFFFF;

  /* send init stream */
  /* send initialization stream */
  REG(MMC0_SD_CON) |= 0x2;
  REG(MMC0_SD_CMD) = 0x0;
  /* wait for command complete flag to be set */
  while (!(REG(MMC0_SD_STAT) & 0x1)) {
    uart_putc('.');
  }
  /* clear SD stat */
  REG(MMC0_SD_STAT) = 0xFFFFFFFF;
  /* end initstream command */
  REG(MMC0_SD_CON) &= ~0x2;

  /* Check CINS to test if card inserted */
  if (!(REG(MMC0_SD_PSTATE) & (0x1 << 16))) {
    uart_puts("!!! no card detected on MMC0\r\n");
    return 1;
  }
  uart_puts("card detected on MMC0\r\n");

  /* reset back to idle state */
  mmc_send_command(0x00, 0x00, 0x00, 0x00);

  /* checking if SD card is compliant with standard 2.0 or later */
  /* send CMD8 (Send Interface Condition Command (see [1] section 4.3.13 )*/
  /* 0x1 signifies that voltage supplied is 2.7-3.6V
     0x55 is "check pattern" that just gets echoed back in response from card */
  if (mmc_send_command(MMC_CMD8_SEND_EXT_CSD, MMC_RSP_48, 0, (0x1 << 8) | (0x55))) {
    /* if command fails then the card does not support SD standard 2.0 */
    return 1;
  }
  /* response should have same check pattern echoed and voltage accpeted high */
  if (REG(MMC0_SD_RSP10) != ((0x1 << 8) | (0x55))) {
    /* echo out response for debugging purposes */
    uart_puts("\r\nRSP10: ");
    uart_hexdump(REG(MMC0_SD_RSP10));
    uart_puts("\r\n");
    uart_puts("card on MMC0 is NOT SD spec v2.0 compliant");
    return 1;
  }

  uart_puts("card on MMC0 is SD spec v2.0 compliant\r\n");

  /* poll OCR register on card [2] 5.1 waiting for powerup routine to finish */
  while (1) {
    /* send app command (has to precede application specific command)*/
    if (mmc_send_command(MMC_CMD55_APP_CMD, MMC_RSP_48_BUSY, 0, 0)) {
      return 1;
    }
    /* send app specific command ACMD41 sends card OCR register back */
    /* argument is host control supported, and VDD voltage window 2.7-3.3V */
    if (mmc_send_command(MMC_ACMD41_SD_SEND_OP_COND, MMC_RSP_48_BUSY, 0,
                         (0x1 << 30) | (0x3F << 15))) {
      return 1;
    }
    /* check powerup routine busy flag, if high then powerup routine is
      completed and we can continue on */
    if (REG(MMC0_SD_RSP10) & (0x1 << 31)) {
      break;
    }
    uart_puts(".");
  }
  uart_puts("SD card powerup completed\r\n");

  /* to get relative card address for all cards, alternate CMD2 and CMD3 for
    each card in the system, we only have 1 card so just do it once */
  /* Send all card IDs command, to put the card into indentification state */
  if (mmc_send_command(MMC_CMD2_ALL_SEND_CID, MMC_RSP_136, 0, 0)) {
    return 1;
  }
  /* get RCA (relative card address) of the first and only connected card */
  if (mmc_send_command(MMC_CMD3_SET_RELATIVE_ADDR, MMC_RSP_48_BUSY, 0, 0)) {
    return 1;
  }
  /* RCA is bits [31:16] of response */
  rca = REG(MMC0_SD_RSP10) >> 16;
  uart_puts("relative card address: ");
  uart_hexdump(rca);
  uart_puts("\r\n");

  /* card csd */
  if (mmc_send_command(MMC_CMD9_SEND_CSD, MMC_RSP_136, 0, (rca << 16))) {
    return 1;
  }

  /* card select */
  if (mmc_send_command(MMC_CMD7_SELECT_CARD, MMC_RSP_48_BUSY, 0, (rca << 16))) {
    return 1;
  }
  uart_puts("Select card completed\r\n");

  /* TODO: set clock frequency back to operating rate */

  return 0;
}
