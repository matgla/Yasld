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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <errno.h>

#include <libopencm3/stm32/usart.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
  putc('c', stdout);
  const char *str = strerror(errno);
  write(STDOUT_FILENO, str, strlen(str));
  char buf[10];
  itoa(errno, buf, 10);
  write(STDOUT_FILENO, buf, strlen(buf));

  usart_send_blocking(USART1, 'w');
  printf("Hello from module!");
  return argc;
}
