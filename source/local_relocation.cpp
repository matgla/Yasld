/**
 * local_relocation.cpp
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

#include "yasld/local_relocation.hpp"

namespace yasld
{

LocalRelocation::LocalRelocation(uint32_t index, uint32_t target_offset)
  : index_(index)
  , target_offset_(target_offset)
{
}

uint32_t LocalRelocation::lot_index() const
{
  return index_ >> 2;
}

const LocalRelocation &LocalRelocation::next() const
{
  return *(this + 1);
}

bool LocalRelocation::operator==(const LocalRelocation &other) const
{
  return index_ == other.index_ && target_offset_ == other.target_offset_;
}

Section LocalRelocation::section() const
{
  return static_cast<Section>(index_ & 0x3);
}

uint32_t LocalRelocation::offset() const
{
  return target_offset_;
}

} // namespace yasld
