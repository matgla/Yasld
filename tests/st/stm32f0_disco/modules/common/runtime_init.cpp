/**
 * runtime_init.cpp
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

#include "runtime_init.hpp"

#include <cstring>

extern "C"
{
  void __libc_init_array();
}

void runtime_init()
{
  extern void (*__preinit_array_start)(void);
  extern void (*__preinit_array_end)(void);
  for (void (**p)(void) = &__preinit_array_start; p < &__preinit_array_end; ++p)
  {
    (*p)();
  }

  extern void (*__init_array_start)(void);
  extern void (*__init_array_end)(void);
  for (void (**p)(void) = &__init_array_start; p < &__init_array_end; ++p)
  {
    (*p)();
  }
}
