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

#include <cstdio>

namespace yasld
{

LocalRelocation::LocalRelocation(uint32_t index, uint32_t offset)
  : index_(index)
  , target_offset_(offset)
{
}

uint32_t LocalRelocation::lot_index() const
{
  return index_ >> 1;
}

const LocalRelocation &LocalRelocation::next() const
{
  return *(this + 1);
}

Section LocalRelocation::section() const
{
  return static_cast<Section>(index_ & 0x1);
}

uint32_t LocalRelocation::offset() const
{
  return target_offset_;
}

Relocation::Relocation(uint32_t index, uint32_t symbol_offset)
  : index_(index)
  , symbol_offset_(symbol_offset)
{
}

uint32_t Relocation::lot_index() const
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

bool Relocation::operator==(const Relocation &other) const
{
  return index_ == other.index_ && symbol_offset_ == other.symbol_offset_;
}

DataRelocation::DataRelocation(uint32_t to, uint32_t from)
  : to_offset_(to)
  , from_offset_(from)
{
}

uint32_t DataRelocation::to_offset() const
{
  return to_offset_;
}

uint32_t DataRelocation::from_offset() const
{
  return from_offset_ >> 1;
}

Section DataRelocation::section() const
{
  return static_cast<Section>(from_offset_ & 1);
}

const DataRelocation &DataRelocation::next() const
{
  return *(this + 1);
}

bool DataRelocation::operator==(const DataRelocation &other) const
{
  return from_offset_ == other.from_offset_ && to_offset_ == other.to_offset_;
}

} // namespace yasld
