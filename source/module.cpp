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

void Module::set_text(const std::span<const std::byte> &text)
{
  text_ = text;
}

void Module::set_exported_symbol_table(const SymbolTable &table)
{
  exported_symbols_ = table;
}

std::span<std::size_t> Module::get_lot()
{
  return { lot_ };
}

std::span<const std::byte> Module::get_text() const
{
  return text_;
}

std::span<std::byte> Module::get_data()
{
  return data_;
}

std::span<std::byte> Module::get_bss()
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

  for (const auto &module : imported_modules_)
  {
    printf("Searching in dependant module %s\n", module->get_name().data());
    auto symbol = module->find_symbol(name);
    if (symbol)
    {
      return symbol;
    }
  }
  return std::nullopt;
}

void Module::save_caller_state(ForeignCallContext ctx)
{
  foreign_call_context_ = ctx;
}

ForeignCallContext Module::restore_caller_state()
{
  return foreign_call_context_;
}

std::span<const std::byte> Module::get_data() const
{
  return data_;
}

std::span<const std::byte> Module::get_bss() const
{
  return bss_;
}

bool Module::allocate_lot(std::size_t lot_size)
{
  lot_.resize(lot_size);
  return lot_size == lot_.size();
}

bool Module::allocate_data(std::size_t data_size, std::size_t bss_size)
{
  data_memory_.resize(data_size + bss_size);
  if (data_memory_.size() != data_size + bss_size)
  {
    return false;
  }

  data_ = std::span<std::byte>(
    data_memory_.begin(), data_memory_.begin() + data_size);
  bss_ =
    std::span<std::byte>(data_memory_.begin() + data_size, data_memory_.end());
  return true;
}

bool Module::allocate_modules(std::size_t number_of_modules)
{
  imported_modules_.reserve(number_of_modules);
  return imported_modules_.capacity() == number_of_modules;
}

Module::ModulesContainer &Module::get_modules()
{
  return imported_modules_;
}

void Module::set_name(const std::string_view &name)
{
  name_ = name;
}

const std::string_view &Module::get_name() const
{
  return name_;
}

std::optional<Module *> Module::find_module_for_program_counter(
  std::size_t program_counter)
{
  {
    const std::size_t text_start = reinterpret_cast<std::size_t>(text_.data());
    const std::size_t text_end   = text_start + text_.size();
    if (program_counter >= text_start && program_counter < text_end)
    {
      return this;
    }
  }
  {
    const std::size_t data_start = reinterpret_cast<std::size_t>(data_.data());
    const std::size_t data_end   = data_start + data_.size();
    if (program_counter >= data_start && program_counter < data_end)
    {
      return this;
    }
  }
  {
    const std::size_t bss_start = reinterpret_cast<std::size_t>(bss_.data());
    const std::size_t bss_end   = bss_start + bss_.size();
    if (program_counter >= bss_start && program_counter < bss_end)
    {
      return this;
    }
  }

  for (auto &module : imported_modules_)
  {
    auto ptr = module->find_module_for_program_counter(program_counter);
    return ptr;
  }
  return std::nullopt;
}

} // namespace yasld
