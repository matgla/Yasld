/**
 * main.cpp
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

#include "board_init.hpp"

#include <cmath>
#include <cstdio>

#include <yasld/environment.hpp>
#include <yasld/loader.hpp>

extern "C"
{
  double __aeabi_f2d(float);
  float  __aeabi_fadd(float, float);
  int    __aeabi_f2iz(float);
  float  __aeabi_fmul(float, float);
}

std::array<float, 3> provide_data_for_test()
{
  return { 3.15f, 23.14123f, -1231.1231f };
}

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  puts("[host] STM32F0 Discovery Board started!");

  yasld::Loader loader(
    [](std::size_t size, yasld::AllocationType)
    {
      return malloc(size);
    },
    [](void *ptr)
    {
      free(ptr);
    });

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{"printf",                    &printf               },
    yasld::SymbolEntry{ "__aeabi_f2d",              &__aeabi_f2d          },
    yasld::SymbolEntry{ "_Z19get_values_for_testv", &provide_data_for_test},
    yasld::SymbolEntry{ "__aeabi_fadd",             &__aeabi_fadd         },
    yasld::SymbolEntry{ "__aeabi_f2iz",             &__aeabi_f2iz         },
    yasld::SymbolEntry{ "sinf",                     &sinf                 },
    yasld::SymbolEntry{ "__aeabi_fmul",             &__aeabi_fmul         },
  };
  loader.set_environment(environment);

  void *module     = reinterpret_cast<void *>(0x08012000);
  auto  executable = loader.load_executable(module);

  if (executable)
  {
    printf("[host] Module loaded\n");
    char  arg[]  = { "executable" };
    char *args[] = { arg };
    int   rc     = (*executable)->execute(1, args);
    printf("[host] Module returned: %d\n", rc);
  }
  else
  {
    printf("[host] Module loading failed\n");
  }

  while (true)
  {
  }
}
