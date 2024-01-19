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

extern "C"
{
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

  loader.set_environment(environment);

  void *module   = reinterpret_cast<void *>(0x08011000);
  auto  exec     = loader.load_executable(module);

  if (exec)
  {
    printf("[host] Module loaded\n");
    char  name[] = { "floopy" };
    char  arg[]  = { "5" };
    char *args[] = { name, arg };
    (*exec)->execute(2, args);
  }

  while (true)
  {
  }
}
