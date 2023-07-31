/**
 * data_relocation.cpp
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

#include "yasld/data_relocation.hpp"

namespace yasld
{

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
