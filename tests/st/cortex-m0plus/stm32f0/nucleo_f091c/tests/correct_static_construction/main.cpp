/**
 * main.cpp
 *
 * Copyright (C) 2024 Mateusz Stadnik <matgla@live.com>
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
#include <cstring>

#include <yasld/environment.hpp>
#include <yasld/loader.hpp>

#include <string_view>

#include "yasld/arch.hpp"
#include "yasld/supervisor_call.hpp"

#include "globals.h"

yasld::Loader *l;

int            main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  init_baselibc_stdout();
  puts("[host] STM32F0 Nucleo Board started!");

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{"stdout", &stdout_file}
  };

  yasld::Loader loader(
    [](std::size_t size, yasld::AllocationType)
    {
      return malloc(size);
    },
    [](void *ptr)
    {
      free(ptr);
    });
  l = &loader;

  loader.set_environment(environment);

  void *module = reinterpret_cast<void *>(0x08011000);
  auto  exec   = loader.load_executable(module);
  if (exec)
  {
    printf("[host] Module loaded\n");
    (*exec)->execute();
  }
  else
  {
    printf("[host] Loading failed\n");
  }
  printf("[host] TEST SUCCESS\n");
  while (true)
  {
  }
}
