/**
 * relocation.hpp
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

namespace yasld
{

class Symbol;

class __attribute__((packed)) Relocation
{
public:
  [[nodiscard]] uint32_t index() const;
  [[nodiscard]] const Symbol &symbol() const;
  [[nodiscard]] uint32_t symbol_offset() const;
  constexpr static std::size_t size()
  {
    return sizeof(Relocation);
  }
  [[nodiscard]] const Relocation &next() const;

private:
  uint32_t index_;
  uint32_t symbol_offset_;
};

} // namespace yasld
