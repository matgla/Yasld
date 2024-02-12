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
#include "yasld/supervisor_call.hpp"

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
  signed __aeabi_idiv(signed numerator, signed denominator);
}

int main(int argc, char *argv[])
{
  static_cast<void>(argc);
  static_cast<void>(argv);
  board_init();
  puts("[host] STM32F0 Discovery Board started!");

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{"printf",        &printf      },
    yasld::SymbolEntry{ "puts",         &puts        },
    yasld::SymbolEntry{ "strlen",       &strlen      },
    yasld::SymbolEntry{ "__aeabi_idiv", &__aeabi_idiv}
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

  void *module  = reinterpret_cast<void *>(0x08011000);
  auto  library = loader.load_library(module);
  if (library)
  {
    printf("[host] Module loaded\n");
    auto sum =
      yasld::SymbolGet<int(int, int)>::get_symbol(**library, "_Z3sumii");
    if (!sum)
    {
      printf("[host] Symbol not found\n");
      while (true)
      {
      }
    }
    else
    {
      printf("[host][sum] Sum is: %d\n", sum(15, 22));
    }

    auto str = yasld::SymbolGet<std::string_view(
      const std::string_view &, const std::string_view &)>::
      get_symbol(
        **library,
        "_ZN1a1b1c11process_strERKSt17basic_string_viewIcSt11char_"
        "traitsIcEES7_");

    if (!str)
    {
      printf("[host] Symbol not found\n");

      while (true)
      {
      }
    }
    else
    {
      printf("[host][process_str] Str is: %s\n", str("hello", "elo").data());
    }

    printf("[host][sum] Other sum is: %d\n", sum(100, 1000));
    printf("[host][process_str] Str is: %s\n", str("other", "test").data());

    auto c_fun = yasld::SymbolGet<int(int, int, int, int, int)>::get_symbol(
      **library, "c_fun");

    if (!c_fun)
    {
      printf("[host] Symbol not found\n");

      while (true)
      {
      }
    }
    else
    {
      printf("[host][c_fun] Calculated: %d\n", c_fun(1, 10, 20, 30, 5));
      printf("[host][c_fun] Calculated: %d\n", c_fun(20, 123, 4, 80, 1000));
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
