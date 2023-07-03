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

int sumi(int count, ...)
{
  int     sum = 0;
  va_list args;
  va_start(args, count);
  for (int i = 0; i < count; ++i)
  {
    int num  = va_arg(args, int);
    sum     += num;
  }
  va_end(args);
  return sum;
}

double summ(float a, float b)
{
  return a + b;
}

int main(int argc, char *argv[])
{
  runtime_init();
  printf("[TEST] STM32F0 Floating Point Test\n");

  double a = 1.5f;
  double b = 2.7f;
  double s = sum(2, a, b);
  printf("%f\n", s);

  char buf[10];
  printf("Print dec: %d\n", 1234);
  printf("%f\n", 1.27f);
  snprintf(buf, 10, "YY: %f", 2.45f);
  printf("Printf spr: %s\n", buf);
  printf("Floating print test: %d, %s, %f\n", 1234, buf, 1.3456);
  float output = 3.0f / 2.0f;
  printf("Floating point division: %f\n", output);
  printf("Errno: %s\n", strerror(errno));
  printf("What after\n");
  return argc;
}
