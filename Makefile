# Copyright (c) 2023  Hunter Whyte

PREFIX= arm-none-eabi-

CC= $(PREFIX)gcc
AS= $(PREFIX)as
LD= $(PREFIX)ld

INC      = include

CFLAGS= -g -mcpu=cortex-a8 -marm -static -ffreestanding -nostdlib -nostartfiles\
  -mfpu=neon -mfloat-abi=hard -mlong-calls
CPPFLAGS= -std=gnu90 -Wall -pedantic -Wextra
ASMFLAGS= -mcpu=cortex-a8 -march=armv7-a

.PHONY: clean

# TODO: fix hardcoded kernel offset/bootloader size of 20 blocks by reading 
# the size of MLO file
boot.img: MLO am335x_header.img
	dd if=am335x_header.img of=boot.img iflag=fullblock
	dd if=MLO of=boot.img iflag=fullblock seek=1

MLO: boot.bin gen_mlo
	./gen_mlo boot.bin MLO

boot.bin: boot.elf
	$(PREFIX)objcopy boot.elf boot.bin -O binary

boot.elf: boot.ld main.o gpio.o uart.o init.o handlers.o timer.o interrupt.o mmc.o
	$(LD) -o boot.elf -T boot.ld main.o gpio.o uart.o handlers.o init.o timer.o interrupt.o mmc.o

handlers.o: handlers.S
	$(AS) -o handlers.o -c $(ASMFLAGS) handlers.S

init.o: init.S
	$(AS) -o init.o -c $(ASMFLAGS) init.S

mmc.o: mmc.c $(INC)/mmc.h $(INC)/common.h $(INC)/prcm.h
	$(CC) -o mmc.o -c $(CFLAGS) $(CPPFLAGS) mmc.c -I$(INC) -I$(INC)

uart.o: uart.c $(INC)/uart.h $(INC)/common.h $(INC)/prcm.h
	$(CC) -o uart.o -c $(CFLAGS) $(CPPFLAGS) uart.c -I$(INC) -I$(INC)

timer.o: timer.c $(INC)/timer.h $(INC)/common.h $(INC)/gpio.h $(INC)/interrupt.h $(INC)/prcm.h
	$(CC) -o timer.o -c $(CFLAGS) $(CPPFLAGS) timer.c -I$(INC) -I$(INC)

gpio.o: gpio.c $(INC)/gpio.h $(INC)/common.h $(INC)/prcm.h
	$(CC) -o gpio.o -c $(CFLAGS) $(CPPFLAGS) gpio.c -I$(INC) -I$(INC)

interrupt.o: interrupt.c $(INC)/interrupt.h $(INC)/common.h $(INC)/timer.h $(INC)/uart.h
	$(CC) -o interrupt.o -c $(CFLAGS) $(CPPFLAGS) interrupt.c -I$(INC) -I$(INC)

main.o: main.c $(INC)/gpio.h $(INC)/common.h $(INC)/prcm.h $(INC)/timer.h $(INC)/uart.h
	$(CC) -o main.o -c $(CFLAGS) $(CPPFLAGS) main.c -I$(INC) -I$(INC)

am335x_header.img: gen_toc
	./gen_toc am335x_header.img

gen_toc: gen_toc.c
	gcc -o gen_toc gen_toc.c

gen_mlo: gen_mlo.c
	gcc -o gen_mlo gen_mlo.c

clean:
	rm *.o *.bin *.elf *.img gen_toc gen_mlo MLO
