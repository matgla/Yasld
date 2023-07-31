/**
 * data_relocation.hpp
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

#include "yasld/section.hpp"

namespace yasld
{

class __attribute__((packed)) DataRelocation
{
public:
  DataRelocation(uint32_t to, uint32_t from);
  [[nodiscard]] uint32_t       to_offset() const;
  [[nodiscard]] uint32_t       from_offset() const;
  [[nodiscard]] Section        section() const;
  constexpr static std::size_t size()
  {
    return sizeof(DataRelocation);
  }

  [[nodiscard]] const DataRelocation &next() const;
  [[nodiscard]] bool operator==(const DataRelocation &other) const;

private:
  uint32_t to_offset_;
  uint32_t from_offset_;
};

} // namespace yasld
