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

#include <cmath>
#include <cstdarg>
#include <cstdio>

double sum(int count, ...)
{
  double  s = 0;
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; ++i)
  {
    double v  = va_arg(args, double);
    s        += v;
  }
  va_end(args);
  return s;
}

int main()
{
  printf("STM32F0 floating point tests\n");
  printf("Print test: %f\n", 1.27);
  double a = 1.5;
  double b = 2.3;
  double c = -7.2;
  printf("Sum is: %f\n", sum(3, a, b, c));
  printf("Division test: %f\n", c / b);
  printf("Sine of %f is %f\n", b, sin(b));
}
