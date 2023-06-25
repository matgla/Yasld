/**
 * relocation_table.hpp
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

#include <cstdint>
#include <span>

namespace yasld
{

class Relocation;

class RelocationTable
{
public:
  RelocationTable(std::uintptr_t address, std::size_t number_of_relocations);

  [[nodiscard]] std::size_t    size() const;
  [[nodiscard]] std::uintptr_t address() const;

  using ConstSpan = std::span<const Relocation>;

  const ConstSpan &span() const;

private:
  ConstSpan relocations_;
};

} // namespace yasld
