/**
 * item_iterator.hpp
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
#include <iterator>

namespace yasld
{

template <typename T>
class ItemIterator
{
public:
  using iterator_category = std::forward_iterator_tag;
  using difference_type   = std::ptrdiff_t;
  using value_type        = T;
  using pointer           = const T *;
  using reference         = const T &;

  ItemIterator(const T *dependency, uint8_t alignment);

  reference     operator*() const;
  pointer       operator->() const;

  ItemIterator &operator++();
  ItemIterator  operator++(int);

  bool          operator==(const ItemIterator &b) const;
  bool          operator!=(const ItemIterator &b) const;

private:
  const uint8_t alignment_;
  const T      *item_;
};

template <typename T>
ItemIterator<T>::ItemIterator(const T *item, uint8_t alignment)
  : alignment_(alignment)
  , item_(item)
{
}

template <typename T>
ItemIterator<T>::reference ItemIterator<T>::operator*() const
{
  return *item_;
}

template <typename T>
ItemIterator<T>::pointer ItemIterator<T>::operator->() const
{
  return item_;
}

template <typename T>
ItemIterator<T> &ItemIterator<T>::operator++()
{
  item_ = item_->next(alignment_);
  return *this;
}

template <typename T>
ItemIterator<T> ItemIterator<T>::operator++(int)
{
  ItemIterator<T> copy = *this;
  item_                = item_->next(alignment_);
  return copy;
}

template <typename T>
bool ItemIterator<T>::operator==(const ItemIterator<T> &b) const
{
  return item_ == b.item_;
}

template <typename T>
bool ItemIterator<T>::operator!=(const ItemIterator<T> &b) const
{
  return item_ != b.item_;
}

} // namespace yasld
