/**
 * symbol.cpp
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

#include "yasld/symbol.hpp"

#include "yasld/align.hpp"

namespace yasld
{

Section Symbol::section() const
{
  return static_cast<Section>(offset_ & 0x3);
}

uint32_t Symbol::offset() const
{
  return offset_ >> 2;
}

std::string_view Symbol::name() const
{
  const char *ptr = reinterpret_cast<const char *>(this + 1);
  return std::string_view(ptr);
}

std::size_t Symbol::size(std::size_t alignment) const
{
  // symbol offset + name + \0
  return sizeof(Symbol) + align<std::size_t>(name().size() + 1, alignment);
}

const Symbol *Symbol::next(std::size_t alignment) const
{
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(this);
  return reinterpret_cast<const Symbol *>(ptr + size(alignment));
}

} // namespace yasld
