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

#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>

std::array<float, 3> get_values_for_test();
static float         sum;

int                  main(int argc, char *argv[])
{
  static_cast<void>(argc);
  auto values = get_values_for_test();
  printf("Module '%s' got array size: %d\n", argv[0], values.size());
  sum = 0;
  for (const auto &data : values)
  {
    printf("%f\n", data);
    sum += std::sin(data) * 10;
  }
  return static_cast<int>(sum);
}
