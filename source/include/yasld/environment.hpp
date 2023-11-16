/**
 * environment.hpp
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

#include <array>
#include <cstdint>
#include <cstdlib>
#include <string_view>

namespace yasld
{

struct SymbolEntry
{
public:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
  SymbolEntry(const std::string_view &symbol_name, auto symbol_address)
    : name{ symbol_name }
    , address{ reinterpret_cast<std::uintptr_t>(
        reinterpret_cast<void *>(symbol_address)) }
  {
  }
#pragma GCC diagnostic pop

  std::string_view name;
  std::uintptr_t   address;
};

class Environment
{
public:
  virtual ~Environment() = default;

  virtual const SymbolEntry *find_symbol(
    const std::string_view &name) const = 0;
};

template <std::size_t N>
class StaticEnvironment : public Environment
{
public:
  template <typename... Args>
  constexpr StaticEnvironment(Args &&...args)
    : entries_({ std::forward<Args>(args)... })
  {
  }

  const SymbolEntry *find_symbol(const std::string_view &name) const override
  {
    for (const auto &symbol : entries_)
    {
      if (symbol.name == name)
      {
        return &symbol;
      }
    }
    return nullptr;
  }

private:
  std::array<SymbolEntry, N> entries_;
};

template <typename... Args>
StaticEnvironment(Args &&...) -> StaticEnvironment<sizeof...(Args)>;

} // namespace yasld
