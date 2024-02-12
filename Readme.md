# Disclaimer 
This project is Work In Progress. 
It works for some examples, but may fail in real usage.

Currently only part of dynamic loader is implemented. Dynamic linking is not working yet.

# About

```yasld``` implements dynamic linker and loader for ARM Cortex-M microcontrollers. 
It can support any binary or library that was compiled as PIC or PIE. 

Loader supports binary execution directly from flash thanks to execute in place if hardware supports that. 
Otherwise binary or library can be copied to RAM and executed from there. 

# Design 

Executable or shared library must be build as ELF file with enabled position independent code. Currently project supports only ```arm-none-eabi``` toolchain. 

To compile module to be supported below compiler and linker flags must be passed:
```
compiler: -Wl,--unresolved-symbols=ignore-in-object-files -Wl,--emit-relocs -fno-function-sections -fno-data-sections -fno-section-anchors -msingle-pic-base -mno-pic-data-is-text-relative -fPIE -mlong-calls
```

But if you are using CMake there is CMake file that pass all necessary flags to build toolchain.

# File format 

Yasdl uses custom file format called yasiff (Yet Another Simple Image File Format).

```
+---------------+
| Y | A | F | F | 4B - header magic cookie 
+---+---+---+---+
| t | arch  | v | 1B t - type (exec/library),
+---+-------+---+ 2B arch - CPU arch, v - yaff version 
|   Code Size   | 4B - size of code section 
+---------------+           in bytes
|   Data Size   | 4B - size of data section 
+---------------+           in bytes
|    BSS Size   | 4B - size of bss section 
+-------+-------+           in bytes
|  dls  | a | r | dls - 2B dependend libraries number
+-------+-------+ a - 1B alignment of memory, res - 1B reserved
| iv mj | iv mn | 2B iv mj/iv mn - image version 
+-------+-------+                  major/minor
|  ers  |  lrs  | 2B ers/lrs - number of external
+-------+-------+          / local relocations 
|  drs  |  xrs  | 2B drs - data relocations amount,
+-------+-------+ 2B xrs - exported relocations amount
|  esa  |  xsa  | 2B esa - amount of exported symbols
+---+---+-------+ 2B xsa - external symbols amount
|               |
.     name      . N bytes - name in ASCII with trailing \0 aligned to alignment
|               |
+---+---+-------+
|               |
.  future arch  . CPU Arch section, unused yet
|               |   needed for arch verification (0-N bytes)
+---------------+
|               |
. dls library   . names in ASCII followed by version  
|   names       | ended with \0 aligned to alignment (0-N bytes) 
+---------------+ 
|               |
.  alignement   . alignment to value in header
|               |
+---------------+
|    symtab     |
.  relocations  . encoded symbol table relocations
|      ...      |
+---------------+
|     local     |
.  relocations  . encoded local relocations
|      ...      |
+---------------+
|     data      |
.  relocations  . encoded data relocations
|      ...      |
+---------------+
|   imported    | 
.   symbols     . imported symbols table 
|      ...      |
+---------------+
|   exported    |
.    symbols    . exported symbols table
|      ...      |
+---------------+
|               |
.   alignment   . alignment to 16 bytes
|               |
+---------------+
|               |
.     code      . code section aligned to 16 bytes 
|               |
+---------------+
|               |
.     data      . data section 
|               |
+---------------+


Symbol Table Relocation Entry
+---------------+
|    index      | 4 bytes - lot index of relocation 
+---------------+
| symtab index  | 4 bytes - index in symbol table 
+---------------+

Local Relocation Entry
+---------------+
|    index    |s| 31 bits - lot index of relocation, 1 bit section flag (code/data)
+---------------+
|    offset     | 4 bytes - relocation offset
+---------------+

Data Relocation Entry
+---------------+
|     to      |s| 31 bits - offset to relocate, 1 bit section flag (code/data)
+---------------+
|     from      | 4 bytes - offset from relocate 
+---------------+

Symbol Table Entry 
+---------------+
|    offset     | 4 byte - offset to symbol
+---------------+
|               |
.     name      . name - name encoded in ASCII with trailing \0, aligned to alignemnt from header
|               |
+---------------+


```

To generate yasff image use ```scripts/mkimage```

# Offsets 

mkimage change relocation position in relocation table. Due to that mkimage must also fix offsets in code.

# Similar projects 

Dynamic Linker with C support: https://github.com/bogdanm/udynlink

