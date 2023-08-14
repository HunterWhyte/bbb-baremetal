/* Copyright (c) 2023  Hunter Whyte */
#ifndef _CONTROL_H
#define _CONTROL_H

#define CONTROL_MODULE_BASE 0x44E10000

#define CONTROL_MODULE_CONTROL_EMIF_SDRAM_CONFIG (CONTROL_MODULE_BASE + 0x110)
#define CONTROL_MODULE_DDR_IO_CTRL (CONTROL_MODULE_BASE + 0xE04)
#define CONTROL_MODULE_VTP_CTRL (CONTROL_MODULE_BASE + 0xE0C)
#define CONTROL_MODULE_DDR_CKE_CTRL (CONTROL_MODULE_BASE + 0x131C)

#define CONTROL_MODULE_DDR_CMD0_IOCTRL (CONTROL_MODULE_BASE + 0x1404)
#define CONTROL_MODULE_DDR_CMD1_IOCTRL (CONTROL_MODULE_BASE + 0x1408)
#define CONTROL_MODULE_DDR_CMD2_IOCTRL (CONTROL_MODULE_BASE + 0x140C)
#define CONTROL_MODULE_DDR_DATA0_IOCTRL (CONTROL_MODULE_BASE + 0x1440)
#define CONTROL_MODULE_DDR_DATA1_IOCTRL (CONTROL_MODULE_BASE + 0x1444)

#define CONTROL_MODULE_CONF_MMC0_DAT3 (CONTROL_MODULE_BASE + 0x8F0)
#define CONTROL_MODULE_CONF_MMC0_DAT2 (CONTROL_MODULE_BASE + 0x8F4)
#define CONTROL_MODULE_CONF_MMC0_DAT1 (CONTROL_MODULE_BASE + 0x8F8)
#define CONTROL_MODULE_CONF_MMC0_DAT0 (CONTROL_MODULE_BASE + 0x8FC)
#define CONTROL_MODULE_CONF_MMC0_CLK  (CONTROL_MODULE_BASE + 0x900)
#define CONTROL_MODULE_CONF_MMC0_CMD  (CONTROL_MODULE_BASE + 0x904)

#define DDR_PHY_BASE 0x44E12000
#define CMD0_REG_PHY_CTRL_SLAVE_RATIO_0     (DDR_PHY_BASE + 0x01C)
#define CMD0_REG_PHY_DLL_LOCK_DIFF_0        (DDR_PHY_BASE + 0x028)
#define CMD0_REG_PHY_INVERT_CLKOUT_0        (DDR_PHY_BASE + 0x02C)
#define CMD1_REG_PHY_CTRL_SLAVE_RATIO_0     (DDR_PHY_BASE + 0x050)
#define CMD1_REG_PHY_DLL_LOCK_DIFF_0        (DDR_PHY_BASE + 0x05C)
#define CMD1_REG_PHY_INVERT_CLKOUT_0        (DDR_PHY_BASE + 0x060)
#define CMD2_REG_PHY_CTRL_SLAVE_RATIO_0     (DDR_PHY_BASE + 0x084)
#define CMD2_REG_PHY_DLL_LOCK_DIFF_0        (DDR_PHY_BASE + 0x090)
#define CMD2_REG_PHY_INVERT_CLKOUT_0        (DDR_PHY_BASE + 0x094)
#define DATA0_REG_PHY_RD_DQS_SLAVE_RATIO_0  (DDR_PHY_BASE + 0x0C8)
#define DATA0_REG_PHY_WR_DQS_SLAVE_RATIO_0  (DDR_PHY_BASE + 0x0DC)
#define DATA0_REG_PHY_WRLVL_INIT_RATIO_0    (DDR_PHY_BASE + 0x0F0)
#define DATA0_REG_PHY_WRLVL_INIT_MODE_0     (DDR_PHY_BASE + 0x0F8)
#define DATA0_REG_PHY_GATELVL_INIT_RATIO_0  (DDR_PHY_BASE + 0x0FC)
#define DATA0_REG_PHY_GATELVL_INIT_MODE_0   (DDR_PHY_BASE + 0x104)
#define DATA0_REG_PHY_FIFO_WE_SLAVE_RATIO_0 (DDR_PHY_BASE + 0x108)
#define DATA0_REG_PHY_DQ_OFFSET_0           (DDR_PHY_BASE + 0x11C)
#define DATA0_REG_PHY_WR_DATA_SLAVE_RATIO_0 (DDR_PHY_BASE + 0x120)
#define DATA0_REG_PHY_USE_RANK0_DELAYS      (DDR_PHY_BASE + 0x134)
#define DATA0_REG_PHY_DLL_LOCK_DIFF_0       (DDR_PHY_BASE + 0x138)
#define DATA1_REG_PHY_RD_DQS_SLAVE_RATIO_0  (DDR_PHY_BASE + 0x16C)
#define DATA1_REG_PHY_WR_DQS_SLAVE_RATIO_0  (DDR_PHY_BASE + 0x180)
#define DATA1_REG_PHY_WRLVL_INIT_RATIO_0    (DDR_PHY_BASE + 0x194)
#define DATA1_REG_PHY_WRLVL_INIT_MODE_0     (DDR_PHY_BASE + 0x19C)
#define DATA1_REG_PHY_GATELVL_INIT_RATIO_0  (DDR_PHY_BASE + 0x1A0)
#define DATA1_REG_PHY_GATELVL_INIT_MODE_0   (DDR_PHY_BASE + 0x1A8)
#define DATA1_REG_PHY_FIFO_WE_SLAVE_RATIO_0 (DDR_PHY_BASE + 0x1AC)
#define DATA1_REG_PHY_DQ_OFFSET_0           (DDR_PHY_BASE + 0x1C0)
#define DATA1_REG_PHY_WR_DATA_SLAVE_RATIO_0 (DDR_PHY_BASE + 0x1C4)
#define DATA1_REG_PHY_USE_RANK0_DELAYS      (DDR_PHY_BASE + 0x1D8)
#define DATA1_REG_PHY_DLL_LOCK_DIFF_0       (DDR_PHY_BASE + 0x1DC)

/* Configuration values taken from TI's starterware as a start
TODO: tuning needed to match actual DDR chip on bbb */
#define DDR3_CMD_SLAVE_RATIO                0x80
#define DDR3_CMD_INVERT_CLKOUT              0x0

#define DDR3_DATA0_RD_DQS_SLAVE_RATIO       0x38
#define DDR3_DATA0_FIFO_WE_SLAVE_RATIO      0x94
#define DDR3_DATA0_WR_DQS_SLAVE_RATIO       0x44
#define DDR3_DATA0_WR_DATA_SLAVE_RATIO      0x7D

#define DDR3_IOCTRL_VALUE                   0x18B

#define DDR3_CONTROL_DDR_IO_CTRL            0xefffffff

#define DDR3_READ_LATENCY                   0x07
#define DDR3_SDRAM_TIMING1                  0x0AAAD4DB
#define DDR3_SDRAM_TIMING2                  0x266B7FDA   
#define DDR3_SDRAM_TIMING3                  0x501F867F
#define DDR3_SDRAM_CONFIG                   0x61C05332

#define DDR3_REF_CTRL                       0x00000C30
#define DDR3_ZQ_CONFIG                      0x50074BE4

#endif /* _CONTROL_H */