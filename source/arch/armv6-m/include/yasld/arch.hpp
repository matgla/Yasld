/**
 * call.hpp
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

#pragma once

#include <cstdlib>

constexpr static int          resolve_lot_svc_id = 10;

static __inline__ std::size_t get_pc(void)
{
  std::size_t pc;
  asm("mov %0, pc" : "=r"(pc));
  return pc;
}

int inline __attribute__((always_inline)) get_lot_svcall()
{
  const std::size_t pc = get_pc();
  asm volatile inline("mov r0, %[id]\t\n"
                      :
                      : [id] "r"(resolve_lot_svc_id)
                      : "r0");
  asm volatile inline("mov r1, %[pc]\t\n" : : [pc] "r"(&pc) : "r1");
  asm volatile inline("svc 0\t\n");
  int rc;
  asm volatile inline("mov %[rc], r0\t\n" : [rc] "=r"(rc));
  return rc;
}

struct ForeignCallContext
{
  std::size_t r9;
  std::size_t lr;
  std::size_t tmpReg[2];
};
