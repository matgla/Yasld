/**
 * allocator.hpp
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

#include <cstdlib>

#include <eul/functional/function.hpp>

namespace yasld
{
using AllocatorType = eul::function<void *(std::size_t size), sizeof(void *)>;
using ReleaseType   = eul::function<void(void *data), sizeof(void *)>;

class YasldAllocatorHolder
{
public:
  static YasldAllocatorHolder &get()
  {
    static YasldAllocatorHolder o;
    return o;
  }

  void           set_allocator(const AllocatorType &allocator);
  void           set_release(const ReleaseType &release);
  AllocatorType &get_allocator();
  ReleaseType   &get_release();

private:
  YasldAllocatorHolder() = default;
  AllocatorType allocator_;
  ReleaseType   release_;
};

template <typename T>
class YasldAllocator
{
public:
  using value_type = T;

  T *allocate(std::size_t n) noexcept
  {
    auto &alloc = YasldAllocatorHolder::get().get_allocator();
    if (alloc)
    {
      return static_cast<T *>(alloc(n * sizeof(T)));
    }
    return nullptr;
  }

  void deallocate(T *p, std::size_t n) noexcept
  {
    static_cast<void>(n);
    auto &release = YasldAllocatorHolder::get().get_release();
    if (release)
    {
      return release(p);
    }
  }
};

template <typename T>
class YasldDeleter
{
public:
  void operator()(T *p) const
  {
    auto &release = YasldAllocatorHolder::get().get_release();
    if (release)
    {
      return release(p);
    }
  }
};

} // namespace yasld
