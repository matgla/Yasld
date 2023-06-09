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

#include <cstdio>
#include <cstring>

namespace yasld
{

Section Symbol::section() const
{
  return section_;
}

uint32_t Symbol::offset() const
{
  return offset_;
}

std::string_view Symbol::name() const
{
  const char *ptr = reinterpret_cast<const char *>(this + 1);
  return std::string_view(ptr);
}

std::size_t Symbol::size() const
{
  return sizeof(Symbol) + name().size();
}

const Symbol *Symbol::next() const
{
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(this);
  return reinterpret_cast<const Symbol *>(ptr + size());
}

} // namespace yasld