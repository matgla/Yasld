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

Relocation::Relocation(uint32_t index, uint32_t symbol_index)
  : index_(index)
  , symbol_index_(symbol_index)
{
}

uint32_t Relocation::lot_index() const
{
  return index_;
}

uint32_t Relocation::symbol_index() const
{
  return symbol_index_;
}

const Relocation &Relocation::next() const
{
  return *(this + 1);
}

bool Relocation::operator==(const Relocation &other) const
{
  return index_ == other.index_ && symbol_index_ == other.symbol_index_;
}

} // namespace yasld
