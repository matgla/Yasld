/**
 * executable.ld
 *
 * Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 */

MEMORY
{
  image (xrw) : ORIGIN = 0x00000000, LENGTH = 0xFFFFFFFF
}

ENTRY(main)

SECTIONS 
{
  .text : 
  {
    . = ALIGN(4);
    KEEP(*(.entry))
    *(.text)
    *(.text*)
    *(.rel.text)
    *(.rel.text*)
    *(.glue_7) /* This is for armv-7, but it's not harmful for rest */
    *(.glue_7t) /*  Same for that */
    *(.eh_frame)
    KEEP(*(.init))
    KEEP(*(.fini))
    *(.rodata)
    *(.rodata*)
    *(.ARM.extab*)
    *(.ARM.exidx*)
    . = ALIGN(4);
    PROVIDE_HIDDEN(__preinit_array_start = .);
    KEEP(*SORT(.preinit_array.*))
    KEEP(*(.preinit_array))
    PROVIDE_HIDDEN(__preinit_array_end = .);
    . = ALIGN(4);
    PROVIDE_HIDDEN(__init_array_start = .);
    KEEP(*SORT(.init_array.*))
    KEEP(*(.init_array))
    PROVIDE_HIDDEN(__init_array_end = .);
    . = ALIGN(4);
    PROVIDE_HIDDEN(__fini_array_start = .);
    KEEP(*SORT(.fini_array.*))
    KEEP(*(.fini_array))
    PROVIDE_HIDDEN(__fini_array_end = .);
    . = ALIGN(4); 
  } > image 

  .data : 
  {
    . = ALIGN(4);
    *(.data)
    *(.data*)
    . = ALIGN(4);
  } > image 

  .bss : 
  {
    . = ALIGN(4);
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
  } > image 

  .got : 
  {
    *(.got*)
  } > image 

}
