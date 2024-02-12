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
  int call_main(int argc, char *argv[], std::size_t address, const void *lot);
} // extern "C"

namespace yasld
{

bool Executable::initialize_main()
{
  main_address_ = find_symbol("main");
  return main_address_.has_value();
}

int Executable::execute(int argc, char *argv[]) const
{
  log("Executing 'main' inside module, lot: %p\n", lot_.data());
  if (!main_address_)
  {
    return -1;
  }
  return call_main(argc, argv, *main_address_, lot_.data());
}

int Executable::execute() const
{
  log("Executing 'main' inside module, lot: %p\n", lot_.data());
  if (!main_address_)
  {
    return -1;
  }
  return call_main(0, nullptr, *main_address_, lot_.data());
}

} // namespace yasld
