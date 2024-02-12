/**
 * item_table.hpp
 *
 * Copyright (C) 2024 Mateusz Stadnik <matgla@live.com>
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

#include "yasld/item_iterator.hpp"

namespace yasld
{

template <typename T>
class ItemTable
{
public:
  ItemTable(
    std::uintptr_t address,
    uint16_t       number_of_items,
    uint8_t        alignment);

  [[nodiscard]] std::uintptr_t address() const;
  [[nodiscard]] std::size_t    size() const;

  ItemIterator<T>              begin() const;
  ItemIterator<T>              end() const;

  const T                     &operator[](uint32_t position) const;

private:
  uint8_t  alignment_;
  uint16_t number_of_items_;
  const T *root_;
};

template <typename T>
ItemTable<T>::ItemTable(
  std::uintptr_t address,
  uint16_t       number_of_items,
  uint8_t        alignment)
  : alignment_(alignment)
  , number_of_items_(number_of_items)
  , root_(reinterpret_cast<const T *>(address))
{
}

template <typename T>
std::size_t ItemTable<T>::size() const
{
  std::size_t size = 0;
  const T    *item = root_;

  for (int i = 0; i < number_of_items_; ++i)
  {
    size += item->size(alignment_);
    item  = item->next(alignment_);
  }
  return size;
}

template <typename T>
std::uintptr_t ItemTable<T>::address() const
{
  return reinterpret_cast<std::uintptr_t>(root_);
}

template <typename T>
ItemIterator<T> ItemTable<T>::begin() const
{
  return { root_, alignment_ };
}

template <typename T>
ItemIterator<T> ItemTable<T>::end() const
{
  return { reinterpret_cast<const T *>(
             reinterpret_cast<uintptr_t>(root_) + size()),
           alignment_ };
}

template <typename T>
const T &ItemTable<T>::operator[](uint32_t position) const
{
  auto b = begin();
  for (uint32_t p = 0; p < position; ++p)
  {
    ++b;
  }
  return *b;
}

} // namespace yasld
