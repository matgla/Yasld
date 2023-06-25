/**
 * relocation.cpp
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

#include "yasld/relocation.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

uint32_t Relocation::index() const
{
  return index_;
}

uint32_t Relocation::symbol_offset() const
{
  return symbol_offset_;
}

const Symbol &Relocation::symbol() const
{
  const uint8_t *ptr  = reinterpret_cast<const uint8_t *>(this);
  ptr                += symbol_offset_;
  return reinterpret_cast<const Symbol &>(*ptr);
}

const Relocation &Relocation::next() const
{
  return *(this + 1);
}

} // namespace yasld
