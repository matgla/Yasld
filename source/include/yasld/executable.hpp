/**
 * executable.hpp
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

#pragma once

#include <cstdlib>

#include "yasld/module.hpp"

namespace yasld
{

class Executable : public Module
{
public:
  using Module::Module;

  bool initialize_main();
  int  execute(int argc, char *argv[]) const;
  int  execute() const;

  void set_entry(std::size_t entry);

private:
  std::optional<std::size_t> main_address_;
  bool                       has_entry_;
};

} // namespace yasld
