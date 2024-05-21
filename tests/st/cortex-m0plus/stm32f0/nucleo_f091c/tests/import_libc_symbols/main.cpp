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
#include <cstring>

#include <yasld/environment.hpp>
#include <yasld/loader.hpp>

#include <string_view>

#include "yasld/arch.hpp"
#include "yasld/supervisor_call.hpp"

#include "baselibc_variables/globals.h"

yasld::Loader *l;

extern "C"
{
  void __attribute__((noinline))
  sv_call_handler(std::size_t svc_id, std::size_t *args)
  {
    switch (svc_id)
    {
    case 10:
    {
      process_entry_supervisor_call(l, args);
    }
    break;
    case 11:
    {
      process_exit_supervisor_call(l, args);
    }
    break;
    };
  }
  signed  __aeabi_idiv(signed numerator, signed denominator);

  ssize_t _write(int fd, const char *buf, size_t count);
}

int main(int argc, char *argv[])
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

  void *module  = reinterpret_cast<void *>(0x08011000);
  auto  library = loader.load_library(module);
  if (library)
  {
    printf("[host] Module loaded\n");
    auto strcpy_f =
      yasld::SymbolGet<char *(char *dest, const char *source)>::get_symbol(
        **library, "strcpy");
    if (!strcpy_f)
    {
      printf("[host] Symbol not found\n");
      while (true)
      {
      }
    }
    else
    {
      const char *test = "this is test string\n";
      char        buffer[50];
      strcpy_f(buffer, test);
      printf("[host][strcpy]: %s\n", buffer);
    }
    auto puts_f =
      yasld::SymbolGet<int(const char *str)>::get_symbol(**library, "puts");
    if (!puts_f)
    {
      printf("[host] Symbol not found\n");
      while (true)
      {
      }
    }
    else
    {
      puts_f("[host][puts] Used puts from shared libc\n");
    }
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
