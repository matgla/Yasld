/**
 * globals.cpp
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

#include "globals.h"

#include <unistd.h>

FILE  *stdout_file;

size_t write_stdout(FILE *instance, const char *bp, size_t n)
{
  (void)instance;
  return write(STDOUT_FILENO, bp, n);
}

void init_baselibc_stdout()
{
  static struct File_methods stdout_fun = { .write = &write_stdout,
                                            .read  = NULL };
  static FILE                stdout_    = { .vmt = &stdout_fun };
  stdout_file                           = &stdout_;
}
