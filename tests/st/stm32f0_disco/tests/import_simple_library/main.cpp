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

#include <cstdio>
#include <string_view>

#include "yasld/arch.hpp"
#include "yasld/svc.hpp"

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
      process_entry_service_call(l, args);
    }
    break;
    case 11:
    {
      process_exit_service_call(l, args);
    }
    break;
    };
  }

  void print_wrapped()
  {
    printf("Hello from printf\n");
  }
}

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  puts("[host] STM32F0 Discovery Board started!");

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{"printf",  &printf},
    yasld::SymbolEntry{ "puts",   &puts  },
    yasld::SymbolEntry{ "strlen", &strlen}
  };

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

  loader.set_environment(environment);

  print_wrapped();
  void *module  = reinterpret_cast<void *>(0x08010000);
  auto  library = loader.load_library(module);

  if (library)
  {
    printf("[host] Module loaded\n");
    auto sum =
      yasld::SymbolGet<int(int, int)>::get_symbol(*library, "_Z3sumii");
    if (!sum)
    {
      printf("Sybmol not found\n");
    }
    else
    {
      printf("[sum] Sum is: %d\n", sum(15, 22));
    }
  }
  else
  {
    printf("[host] Loading failed\n");
  }

  while (true)
  {
  }
}
