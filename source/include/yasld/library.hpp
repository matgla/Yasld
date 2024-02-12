/**
 * library.hpp
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

#include "yasld/module.hpp"

namespace yasld
{
class Library : public Module
{
public:
  using Module::Module;

  template <typename T>
  T get_symbol(const std::string_view &name)
  {
    auto address = find_symbol(name);
    return reinterpret_cast<T>(address);
  }
};

template <class R, class... Args>
class SymbolGet;

template <class R, class... Args>
class SymbolGet<R(Args...)>
{
public:
  using F = R (*)(Args...);

  static F get_symbol(const Library &library, const std::string_view &name)
  {
    auto symbol = library.find_symbol(name);
    if (!symbol)
    {
      return nullptr;
    }
    return reinterpret_cast<R (*)(Args...)>(*symbol);
  }
};

} // namespace yasld
