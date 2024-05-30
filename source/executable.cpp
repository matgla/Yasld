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
  int call_entry(std::size_t address, const void *lot);
  int call_main(int argc, char *argv[], std::size_t address, const void *lot);
} // extern "C"

namespace yasld
{

void Executable::set_entry(std::size_t entry)
{
  log("Setting executable entry to: 0x%x\n", entry);
  main_address_ = entry;
  has_entry_    = true;
}

bool Executable::initialize_main()
{
  if (has_entry_)
  {
    return true;
  }
  main_address_ = find_symbol("main");
  return main_address_.has_value();
}

int Executable::execute(int argc, char *argv[]) const
{
  log(
    "Executing 'main' inside module, lot: %p, text: %p\n",
    lot_.data(),
    text_.data());
  if (!main_address_ || has_entry_)
  {
    return -1;
  }
  return call_main(argc, argv, *main_address_, lot_.data());
}

int Executable::execute() const
{
  if (has_entry_)
  {
    log(
      "Executing from entry point '%x', lot: %p, text: %p\n",
      *main_address_,
      lot_.data(),
      text_.data());

    return call_entry(*main_address_, lot_.data());
  }
  else
  {
    log(
      "Executing 'main' inside module, lot: %p, text: %p\n",
      lot_.data(),
      text_.data());
    if (!main_address_)
    {
      return -1;
    }
    return call_main(0, nullptr, *main_address_, lot_.data());
  }
}

} // namespace yasld
