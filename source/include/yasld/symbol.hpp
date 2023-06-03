/**
 * symbol.hpp
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
#include <string_view>

namespace yasld
{

enum class Section : uint8_t
{
  code = 0,
  data = 1,
  unknown = 2
};

class __attribute__((packed)) Symbol
{
public:
  [[nodiscard]] Section section() const;
  [[nodiscard]] uint32_t offset() const;
  [[nodiscard]] std::string_view name() const;
  [[nodiscard]] const Symbol *next() const;
  [[nodiscard]] std::size_t size() const;

private:
  Section section_;
  uint8_t _r1;
  uint16_t _r2;
  uint32_t offset_;
};

} // namespace yasld
