/* Copyright (c) 2023  Hunter Whyte */
/* basic linker script for SPL */

/* See table 2.1 in AM335x TRM for memory map */
MEMORY
{
    /* includes internal SRAM and OCMC0 64kB each
    first 1kB of internal SRAM is reserved */
    internal_ram : ORIGIN = 0x402F0400, LENGTH = 0x1FBFF
}

ENTRY(entry)

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        init.o (.text)
        *(.text*)
        *(.rodata*)
    } > internal_ram

    .data :
    {
        . = ALIGN(4);
        *(.data*)
    } > internal_ram

    .bss :
    {
        . = ALIGN(4);
         /* assign a symbol to current location counter so that init code knows 
         where to start and end for clearing bss to 0 */
        _begin_bss = .;
        *(.bss*)
        *(COMMON)
        _end_bss = .;
    } > internal_ram

    .stack :
    {
        . = ALIGN(256);
        _stack_limit = . ;
        *(.stack*)
        . = . + 0x800; /* manually give 0x800 bytes for stack */
        _stack_top = .;
    } > internal_ram
}