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

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>
#include <format>

#include <libopencm3/stm32/usart.h>

#include <unistd.h>

#include "runtime_init.hpp"

double sum(int count, ...)
{
  double  sum = 0;
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; ++i)
  {
    double num  = va_arg(args, double);
    sum        += num;
  }
  va_end(args);
  return sum;
}

int main(int argc, char *argv[])
{
  runtime_init();

  printf("[TEST] STM32F0 Floating Point Test\n");
  printf("[TEST] Print test: %f\n", 1.27f);
  double a = 1.5f;
  double b = 2.7f;
  double s = sum(2, a, b);
  printf("[TEST] Sum of %.2f and %.2f is %.2f\n", a, b, s);
  printf("[TEST] Floating point division: %.2f\n", b / a);

  printf("[TEST] Errno is: %s\n", strerror(errno));
  printf("[TEST] End\n");
  return argc;
}
