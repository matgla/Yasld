/**
 * executable.cpp
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

#include "yasld/executable.hpp"

#include "yasld/logger.hpp"

extern "C"
{
  int call_main(int argc, char *argv[], std::size_t address, void *lot);
}

namespace yasld
{

Executable::Executable(
  const std::size_t      start_address,
  const std::size_t      text_address,
  std::span<std::size_t> lot)
  : start_address_(start_address)
  , text_address_(text_address)
  , lot_(lot)
{
}

int Executable::execute(int argc, char *argv[]) const
{
  log("Executing module\n");
  return call_main(argc, argv, start_address_, lot_.data());
}

std::size_t Executable::text_address() const
{
  return text_address_;
}

} // namespace yasld
