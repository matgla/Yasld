/**
 * module.hpp
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

#include <optional>
#include <span>
#include <string_view>

#include "yasld/arch.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

// Base for executable and library
class Module
{
public:
  virtual ~Module() = default;
  Module();

  void                    set_lot(const std::span<std::size_t> &lot);
  void                    set_text(const std::span<const std::byte> &text);
  void                    set_data(const std::span<std::byte> &data);
  void                    set_bss(const std::span<std::byte> &bss);
  void                    set_exported_symbol_table(const SymbolTable &table);

  std::span<std::size_t> &get_lot();
  std::span<const std::byte>        get_text() const;
  std::span<std::byte>              get_data();
  std::span<std::byte>              get_bss();
  std::span<const std::byte>        get_data() const;
  std::span<const std::byte>        get_bss() const;
  const std::optional<SymbolTable> &get_exported_symbol_table() const;
  std::optional<std::size_t> find_symbol(const std::string_view &name) const;

  // If call from foreign module previous R9 and PC counter inside wrapper must
  // be saved
  // Works thanks to limitation that shared libraries cannot contain
  // cyclic dependency
  void                       save_caller_state(ForeignCallContext ctx);
  ForeignCallContext         restore_caller_state();

protected:
  std::span<std::size_t>     lot_;
  std::span<const std::byte> text_;
  std::span<std::byte>       data_;
  std::span<std::byte>       bss_;
  std::optional<SymbolTable> exported_symbols_;
  ForeignCallContext         foreignCallContext_;
};

} // namespace yasld
