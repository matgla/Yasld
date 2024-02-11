/**
 * spawn_cat.cpp
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

#include "spawn_cat.hpp"

#include <cstdio>
#include <unistd.h>

// taken from https://www.asciiart.eu/animals/cats
static const char *cat0 = "|\\---/|\n"
                          "| o_o |\n"
                          " \\_^_/\n";

static const char *cat1 = "      |\\      _,,,---,,_\n"
                          "ZZZzz /,`.-'`'    -.  ;-;;,_\n"
                          "     |,4-  ) )-,_. ,\\ (  `'-'\n"
                          "    '---''(_/--'  `-'\\_)  Felix Lee\n";

static const char *cat2 = "    |\\__/,|   (`\\\n"
                          "  _.|o o  |_   ) )\n"
                          "-(((---(((--------\n";

extern "C"
{
void               spawn_cat(int id, const char *prefix)
{
  id = id % 3;
  const char *cat = cat0;
  switch (id)
  {
  case 0:
  {
    cat = cat0;
  }
  break;
  case 1:
  {
    cat = cat1;
  }
  break;
  case 2:
  {
    cat = cat2;
  }
  break;
  }

  printf("%s is spawning cat %d\n", prefix, id);
  printf("------------------------------------\n");
  printf("%s", cat);
  printf("------------------------------------\n");
}
} // extern "C"
