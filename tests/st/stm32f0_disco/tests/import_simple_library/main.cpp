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

#include <cstdio>
#include <string_view>

#include "yasld/arch.hpp"

yasld::Loader *l;

extern "C"
{
  void __attribute__((noinline))
  sv_call_handler(std::size_t svc_id, std::size_t *args)
  {
    printf("SVC triggered: %d\n", svc_id);
    switch (svc_id)
    {
    case 10:
    {
      printf("Program Counter: %lx\n", static_cast<uint32_t>(args[0]));
      l->save(ForeignCallContext{ .r9 = 0xabcdef, .pc = args[1] });
    }
    break;
    case 11:
    {
      printf("Restore\n");
      ForeignCallContext c = l->restore();
      args[0]              = c.pc;
      args[1]              = c.r9;
    }
    break;
    };
  }
}

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
  l = &loader;

  // void *module = reinterpret_cast<void *>(0x08010000);
  //  auto  executable = loader.load_library(module);

  // if (executable)
  // {
  //   printf("[host] Module loaded\n");
  //   char  arg[]  = { "executable" };
  //   char *args[] = { arg };
  //   executable->execute(1, args);
  // }
  // else
  // {
  //   printf("[host] Module loading failed\n");
  // }

  while (true)
  {
  }
}
