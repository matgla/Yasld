/**
 * simple_library.cpp
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

#include "simple_library.hpp"

#include <cstdio>

static int counter = 0;

int        sum(int a, int b)
{
  printf("[simple_library] Calculating sum %d + %d\n", a, b);
  printf("[simple_library] Call count: %d\n", counter++);
  return a + b;
}

int Foo::bar(int a, int b, int c, int d)
{
  printf("[simple_library] Foo::bar()\n");
  printf("[simple_library] Call count: %d\n", counter++);

  return a + b + c + d;
}

namespace a::b::c
{
std::string_view __attribute__((visibility("default")))
process_str(const std::string_view &a, const std::string_view &b)
{
  printf("[simple_library] A: %s, B: %s\n", a.data(), b.data());
  printf("[simple_library] Call count: %d\n", counter++);
  return "aa";
}

} // namespace a::b::c

extern "C"
{
  int __attribute__((visibility("default")))
  c_fun(int a, int b, int c, int d, int e)
  {
    printf(
      "[simple_library] Got arguments using stack: %d, %d, %d, %d, %d \n",
      a,
      b,
      c,
      d,
      e);
    printf("[simple_library] Call count: %d\n", counter++);

    return a * b * c * d / e;
  }
}
