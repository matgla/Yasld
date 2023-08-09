# Disclaimer 
This project is Work In Progress. 
It works for some examples, but may fail in real usage.

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
| dls   |  res  | dls - 2B dependend libraries number
+-------+-------+ res - 2B reserved
| iv mj | iv mn | 2B iv mj/iv mn - image version 
+-------+-------+                  major/minor
|  ers  |  lrs  | 2B ers/lrs - number of external
+-------+-------+          / local relocations 
|  drs  |  epr  | 2B drs - number of data relocations,
+-------+-------+ 2B epr - number of external symbols 
|               |
.  future arch  . CPU Arch section, unused yet
|               |   needed for arch verification
+---------------+
|               |
. dls library   .   names in ASCII followed by versio  
|   names       |   ended with \0 aligned to 4 
+---------------+
|               |
.   symbols     . symbol table 
|               |  
+---------------+
|               |
.  relocations  . (ers + lrs + drs) relocation headers |               |     - encoded local relocations 
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


Relocation header 

+---------------+
|    index      | 4 bytes - index of relocation 
+---------------+
|    offset     | 4 bytes - offset to symbol 
+---------------+

Symbol header

+---+-----------+
| s |           | s - 1B section to which symbol  
+---+           | belongs
|               |
.     name      . name of symbol in ASCII with 
|               | \0 at end
+---------------+

Symbol Table 

+---------------+
|               |
. Exported Symb . aligned to 4 
|               |
+---------------+
|               |
. External Symb .
|               |
+---------------+

Relocation table 
+---------------+
|               |
. exported rels .
|               |
+---------------+
|               |
.  external rel .
|               |
+---------------+
|               |
. local relocs  .
|               |
+---------------+
|               |
.  data relocs  .
|               |
+---------------+

```

To generate yasff image use ```scripts/mkimage```

# Offsets 

mkimage change relocation position in relocation table. Due to that mkimage must also fix offsets in code.

# Similar projects 

Dynamic Linker with C support: https://github.com/bogdanm/udynlink

