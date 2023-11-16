/**
 * symbol_table.cpp
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

#include "yasld/symbol_table.hpp"

#include "yasld/symbol.hpp"

namespace yasld
{

SymbolTable::SymbolTable(
  std::uintptr_t address,
  uint16_t       number_of_symbols,
  uint8_t        alignment)
  : alignment_(alignment)
  , number_of_symbols_(number_of_symbols)
  , root_(reinterpret_cast<const Symbol *>(address))
{
}

std::size_t SymbolTable::size() const
{
  std::size_t   size   = 0;
  const Symbol *symbol = root_;

  for (int i = 0; i < number_of_symbols_; ++i)
  {
    size   += symbol->size(alignment_);
    symbol  = symbol->next(alignment_);
  }
  return size;
}

std::uintptr_t SymbolTable::address() const
{
  return reinterpret_cast<std::uintptr_t>(root_);
}

SymbolIterator SymbolTable::begin() const
{
  return SymbolIterator(root_, alignment_);
}

SymbolIterator SymbolTable::end() const
{
  return SymbolIterator(
    reinterpret_cast<const Symbol *>(
      reinterpret_cast<uintptr_t>(root_) + size()),
    alignment_);
}

const Symbol &SymbolTable::operator[](uint32_t position) const
{
  auto b = begin();
  for (uint32_t p = 0; p < position; ++p)
  {
    ++b;
  }
  return *b;
}

} // namespace yasld
