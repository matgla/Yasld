// 
// entry.S
// 
// Copyright (C) 2024 Mateusz Stadnik <matgla@live.com>
// 
// This program is free software: you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, either version
// 3 of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be
// useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
// PURPOSE. See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General
// Public License along with this program. If not, see
// <https://www.gnu.org/licenses/>.
// 

.thumb
.syntax unified
.arch armv6-m
.cpu cortex-m0plus

.global _entry 
.thumb_func
.type _entry, %function
_entry:
  push {r4, r5, r6, lr} 
  mov r4, r9 
  adr r5, runtime_init_symbol
  ldr r5, [r5]
  ldr r5, [r4, r5] 
  blx r5 
  adr r5, main_symbol
  ldr r5, [r5]
  ldr r5, [r4, r5]
  blx r5
  pop {r4, r5, r6, pc}

.align 4
runtime_init_symbol:
  .word runtime_init(GOT)
main_symbol:
  .word main(GOT)

