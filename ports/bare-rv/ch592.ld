/* GNU linker script for WCH CH592 */

ENTRY(Reset_Handler)

__stack_size = 512;

MEMORY
{
    FLASH (rx)  : ORIGIN = 0x00000000, LENGTH = 448K
    RAM (xrw)   : ORIGIN = 0x20000000, LENGTH = 26K
}

SECTIONS
{
    .init :
    {
        . = ALIGN(4);
        KEEP(*(.init))
        . = ALIGN(4);
    } >FLASH

    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
        _etext = .;
    } >FLASH

    .dalign :
    {
        . = ALIGN(4);
        _sidata = .;
    } >FLASH

    .data : AT ( _sidata )
    {
        . = ALIGN(4);
        _sdata = .;
        *(.data)
        *(.data*)
        . = ALIGN(8);
        PROVIDE( __global_pointer$ = . + 0x800 );
        *(.sdata)
        *(.sdata*)
        . = ALIGN(4);
        _edata = .;
    } >RAM

    .bss :
    {
        . = ALIGN(4);
        _sbss = .;
        *(.sbss*)
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } >RAM

    .heap :
    {
        . = ALIGN(8);
        _heap_start = .;
    } >RAM

    .stack ORIGIN(RAM) + LENGTH(RAM) - __stack_size :
    {
        . = ALIGN(4);
        _susrstack = .;
        . = . + __stack_size;
        _eusrstack = .;
    } >RAM
}
