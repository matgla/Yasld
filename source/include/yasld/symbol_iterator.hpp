/**
 * symbol_iterator.hpp
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
#include <iterator>

namespace yasld
{

class Symbol;

class SymbolIterator
{
public:
  SymbolIterator(const Symbol *symbol, uint8_t alignment);
  using iterator_category = std::forward_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = Symbol;
  using pointer           = const Symbol *;
  using reference         = const Symbol &;

  reference       operator*() const;
  pointer         operator->();

  SymbolIterator &operator++();
  SymbolIterator  operator++(int);

  bool            operator==(const SymbolIterator &b) const;
  bool            operator!=(const SymbolIterator &b) const;

private:
  const uint8_t alignment_;
  const Symbol *symbol_;
};

} // namespace yasld
