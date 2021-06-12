
// This file is part of MSOS project. This is simple OS for embedded development devices.
// Copyright (C) 2019 Mateusz Stadnik
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

.thumb
.syntax unified
.arch armv6-m


.global call_external
.thumb_func
.align 1
.type call_external, %function
call_external:
    push    {r4, lr}
    mov     r4, r9 
    push    {r4}
    mov     r9, r1
    blx     r0
    pop     {r4} 
    mov     r9, r4
    pop     {r4, pc}

.global call_external_with_args
.thumb_func
.align 1
.type call_external_with_args, %function
call_external_with_args:
    push    {r4, lr}
    mov     r4, r9
    push    {r4}
    mov     r9, r3
    blx     r2
    pop     {r4}
    mov     r9, r4
    pop     {r4, pc}

