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

SymbolTable::SymbolTable(std::size_t number_of_symbols, const Symbol *symbol_table_root_)
  : number_of_symbols_(number_of_symbols)
  , root_(symbol_table_root_)
{
}

std::size_t SymbolTable::size() const
{
  std::size_t   size   = 0;
  const Symbol *symbol = root_;
  for (int i = 0; i < number_of_symbols_; ++i)
  {
    size   = symbol->size();
    symbol = symbol->next();
  }
  return size;
}

} // namespace yasld