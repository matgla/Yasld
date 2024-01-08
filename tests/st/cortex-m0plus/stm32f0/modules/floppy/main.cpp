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

#include <cstdio>
#include <cstdlib>
#include <unistd.h>

extern "C"
{
  void do_stupid_things(const char *prefix);
  void greet_youtube_fans(const char *prefix);
} // extern "C"

void rule_world()
{
  static int times = 0;
  printf("[floppy] rules the world %d time\n", times++);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    return -1;
  }
  const char *prefix = "[floppy]";
  printf("[floppy] starts execution\n");
  int loops = std::atoi(argv[1]);
  for (int i = 0; i < loops; ++i)
  {
    do_stupid_things(prefix);
    greet_youtube_fans(prefix);
    rule_world();
    sleep(1);
  }
}