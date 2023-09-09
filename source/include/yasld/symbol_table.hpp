/**
 * symbol_table.hpp
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

#include "yasld/symbol_iterator.hpp"

namespace yasld
{

class Symbol;

class SymbolTable
{
public:
  SymbolTable(
    std::uintptr_t address,
    uint16_t       number_of_symbols,
    uint8_t        alignment);

  [[nodiscard]] std::uintptr_t address() const;
  [[nodiscard]] std::size_t    size() const;

  SymbolIterator               begin() const;
  SymbolIterator               end() const;

private:
  uint8_t       alignment_;
  uint16_t      number_of_symbols_;
  const Symbol *root_;
};

} // namespace yasld
