/**
 * module.cpp
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

#include "yasld/module.hpp"

#include "yasld/logger.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

Module::Module()
  : lot_{}
  , text_{}
  , data_{}
  , bss_{}
  , exported_symbols_{}
{
}

void Module::set_lot(const std::span<std::size_t> &lot)
{
  lot_ = lot;
}

void Module::set_text(const std::span<const std::byte> &text)
{
  text_ = text;
}

void Module::set_data(const std::span<std::byte> &data)
{
  data_ = data;
}

void Module::set_bss(const std::span<std::byte> &bss)
{
  bss_ = bss;
}

void Module::set_exported_symbol_table(const SymbolTable &table)
{
  exported_symbols_ = table;
}

std::span<std::size_t> &Module::get_lot()
{
  return lot_;
}

const std::span<const std::byte> &Module::get_text() const
{
  return text_;
}

std::span<std::byte> &Module::get_data()
{
  return data_;
}

std::span<std::byte> &Module::get_bss()
{
  return bss_;
}

const std::optional<SymbolTable> &Module::get_exported_symbol_table() const
{
  return exported_symbols_;
}

std::optional<std::size_t> Module::find_symbol(
  const std::string_view &name) const
{
  for (const auto &symbol : *exported_symbols_)
  {
    if (symbol.name() == name)
    {
      const std::size_t base_address =
        symbol.section() == Section::code
          ? reinterpret_cast<std::size_t>(text_.data())
          : reinterpret_cast<std::size_t>(data_.data());
      const std::size_t address = base_address + symbol.offset();
      log("Found symbol '%s' at: 0x%lx\n", symbol.name().data(), address);
      return address;
    }
  }
  return std::nullopt;
}

} // namespace yasld
