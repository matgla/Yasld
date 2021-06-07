MEMORY
{
    image    (xrw)  : ORIGIN = 0x00000000, LENGTH = 0xFFFFFFFF
}

SECTIONS
{
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text*)
        *(.glue_7)
        *(.glue_7t)
        *(.eh_frame)
        KEEP(*(.init))
        KEEP(*(.fini))
        *(.rodata)
        *(.rodata*)
        . = ALIGN(4);
    } >image

    .data :
    {
        . = ALIGN(4);
        *(.data)
        *(.data*)
        . = ALIGN(4);
    } >image

    .bss :
    {
        . = ALIGN(4);
        *(.bss)
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
    } >image

    .got :
    {
        *(.got*)
    }
}
