/**
 * symbol_iterator.cpp
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

#include "yasld/symbol_iterator.hpp"

#include "yasld/symbol.hpp"

namespace yasld
{

SymbolIterator::SymbolIterator(const Symbol *symbol, uint8_t alignment)
  : symbol_(symbol)
  , alignment_(alignment)
{
}

SymbolIterator::reference SymbolIterator::operator*() const
{
  return *symbol_;
}

SymbolIterator::pointer SymbolIterator::operator->()
{
  return symbol_;
}

SymbolIterator &SymbolIterator::operator++()
{
  symbol_ = symbol_->next(alignment_);
  return *this;
}

SymbolIterator SymbolIterator::operator++(int)
{
  SymbolIterator copy = *this;
  symbol_             = symbol_->next(alignment_);
  return copy;
}

bool SymbolIterator::operator==(const SymbolIterator &b) const
{
  return symbol_ == b.symbol_;
}

bool SymbolIterator::operator!=(const SymbolIterator &b) const
{
  return symbol_ != b.symbol_;
}

} // namespace yasld
