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

#include <cstdio>

#include <yasld/loader.hpp>

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  puts("[host] STM32F0 Discovery Board started!");

  yasld::Loader loader(
    [](std::size_t size)
    {
      return malloc(size);
    },
    [](void *ptr)
    {
      free(ptr);
    });

  void *module     = reinterpret_cast<void *>(0x08010000);
  auto  executable = loader.load_executable(module);

  if (executable)
  {
    printf("[host] Module loaded\n");
    char  arg[]  = { "sum_module" };
    char  arg1[] = "20";
    char  arg2[] = "10";
    char  arg3[] = "1";
    char *args[] = { arg, arg1, arg2, arg3 };

    int   rc     = executable->execute(4, args);
    printf("[host] Returned value: %d\n", rc);
  }
  else
  {
    printf("[host] Module loading failed\n");
  }

  while (true)
  {
  }
}
