/**
 * system_stubs.cpp
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

#include <unistd.h>

#include <array>
#include <cstdint>
#include <cstdio>

#include <libopencm3/stm32/usart.h>

static std::array<uint8_t, 1024 * 2> heap;

extern "C"
{
  int _kill(pid_t, int)
  {
    return 0;
  }

  pid_t _getpid(void)
  {
    return 0;
  }

  int _fstat(int, struct stat *)
  {
    return 0;
  }

  int _isatty(int)
  {
    return 0;
  }

  void _exit(int)
  {
    while (true)
    {
    }
  }

  int _close(int)
  {
    return 0;
  }

  off_t _lseek(int, off_t, int)
  {
    return 0;
  }

  ssize_t _read(int, void *, size_t)
  {
    return 0;
  }

  ssize_t _write(int fd, const char *buf, size_t count)
  {
    usart_send_blocking(USART1, 'd');
    for (size_t i = 0; i < count; ++i)
    {
      usart_send_blocking(USART1, buf[i]);
    }
    return count;
  }

  static std::size_t current_heap_end = 0;
  void              *_sbrk(intptr_t incr)
  {
    usart_send_blocking(USART1, 'r');

    if (current_heap_end + incr > heap.size())
    {
      usart_send_blocking(USART1, 'n');
      return nullptr;
    }

    std::size_t prev  = current_heap_end;
    current_heap_end += incr;
    return &heap[prev];
  }

  int _printf_float(
    struct _reent      *data,
    struct _prt_data_t *pdata,
    FILE               *fp,
    int (*pfunc)(struct _reent *, FILE *, const char *, size_t len),
    va_list *ap)
  {
    usart_send_blocking(USART1, 'p');
    usart_send_blocking(USART1, '\n');
    return 0;
  }
}
