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

template <typename T>
class RelocationTable
{
public:
  RelocationTable(std::uintptr_t address, std::size_t number_of_relocations)
    : relocations_{ reinterpret_cast<const T *>(address),
                    number_of_relocations }
  {
  }

  [[nodiscard]] std::size_t size() const
  {
    return relocations_.size() * sizeof(T);
  }
  [[nodiscard]] std::uintptr_t address() const
  {
    return reinterpret_cast<std::uintptr_t>(relocations_.data());
  }

  using ConstSpan = std::span<const T>;

  const ConstSpan &span() const
  {
    return relocations_;
  }

private:
  ConstSpan relocations_;
};

} // namespace yasld
