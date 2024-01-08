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

#include <unistd.h>

#include <yasld/environment.hpp>
#include <yasld/loader.hpp>

#include <string_view>

#include "yasld/arch.hpp"
#include "yasld/service_call.hpp"

#include <libopencm3/cm3/systick.h>

#include "globals.h"

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
      yasld::process_entry_service_call(l, args);
    }
    break;
    case 11:
    {
      yasld::process_exit_service_call(l, args);
    }
    break;
    };
  }

  void do_stupid_things(const char *prefix)
  {
    printf("%s: is doing stupid things\n", prefix);
  }
  void greet_youtube_fans(const char *prefix)
  {
    printf("%s: is greeting YouTube fans\n", prefix);
    static const char *text =
      "   _____               _   _              __     __      _______    _   "
      "         ______              _ \n"
      "  / ____|             | | (_)             \\ \\   / /     |__   __|  | "
      "|          |  ____|            | |\n"
      " | |  __ _ __ ___  ___| |_ _ _ __   __ _   \\ \\_/ /__  _   _| |_   _| "
      "|__   ___  | |__ __ _ _ __  ___| |\n"
      " | | |_ | '__/ _ \\/ _ \\ __| | '_ \\ / _` |   \\   / _ \\| | | | | | | "
      "| '_ \\ / _ \\ |  __/ _` | '_ \\/ __| |\n"
      " | |__| | | |  __/  __/ |_| | | | | (_| |    | | (_) | |_| | | |_| | "
      "|_) |  __/ | | | (_| | | | \\__ \\_|\n"
      "  \\_____|_|  \\___|\\___|\\__|_|_| |_|\\__, |    |_|\\___/ "
      "\\__,_|_|\\__,_|_.__/ \\___| |_|  \\__,_|_| |_|___(_)\n"
      "                                    __/ |                               "
      "                               \n"
      "                                   |___/                                "
      "                               \n";
    printf("%s", text);
  }

  volatile uint32_t system_millis = 0;

  unsigned int      sleep(unsigned int seconds)
  {
    uint32_t start = system_millis;
    while (start + seconds * 1000 > system_millis)
    {
    }
    return system_millis - start;
  }

  void sys_tick_handler(void)
  {
    system_millis = system_millis + 1;
  }
}

void systick_setup()
{
  systick_set_reload(240000);
  systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
  systick_counter_enable();
  systick_interrupt_enable();
}

int main()
{
  board_init();
  systick_setup();
  puts("[host] Runtime System was started!");

  const yasld::StaticEnvironment environment{
    yasld::SymbolEntry{"stdout",              &stdout_file       },
    yasld::SymbolEntry{ "do_stupid_things",   &do_stupid_things  },
    yasld::SymbolEntry{ "greet_youtube_fans", &greet_youtube_fans},
    yasld::SymbolEntry{ "sleep",              &sleep             },
    yasld::SymbolEntry{ "printf",             &printf            },
    yasld::SymbolEntry{ "puts",               &puts              },
    yasld::SymbolEntry{ "atoi",               &atoi              },
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

  void *module   = reinterpret_cast<void *>(0x08011000);
  auto  exec     = loader.load_executable(module);
  auto  lib      = loader.load_library(reinterpret_cast<void *>(0x08012000));
  if (exec && lib)
  {
    printf("[host] Module loaded\n");
    char  name[] = { "floopy" };
    char  arg[]  = { "1" };
    char *args[] = { name, arg };
    (*exec)->execute(2, args);
    printf("/*******************************************/\n");
    printf("/*     NOW IT'S TIME FOR RUNTIME SYSTEM    */\n");
    printf("/*******************************************/\n");
    auto spawn_cat =
      yasld::SymbolGet<void(int id, const char *prefix)>::get_symbol(**lib, "spawn_cat");
    spawn_cat(0, "host");
    sleep(1);
    spawn_cat(1, "host_rulez");
  }

  while (true)
  {
  }
}