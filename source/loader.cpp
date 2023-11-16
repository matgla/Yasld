/**
 * loader.cpp
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

#include "yasld/loader.hpp"

#include <cstring>

#include "yasld/environment.hpp"
#include "yasld/header.hpp"
#include "yasld/logger.hpp"
#include "yasld/parser.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

Loader::Loader(const AllocatorType &allocator, const ReleaseType &release)
  : allocator_{ allocator }
  , release_{ release }
  , lot_{}
  , text_{}
  , data_{}
  , bss_{}
  , exported_symbols_{ std::nullopt }
  , environment_{ nullptr }
{
}

void Loader::set_environment(const Environment &environment)
{
  environment_ = &environment;
}

std::optional<Executable> Loader::load_executable(const void *module_address)
{
  log("Loading exectuable from address: %p\n", module_address);

  const Header *header = process_header(module_address);
  if (!header)
  {
    return std::nullopt;
  }

  const Parser      parser(header);
  const std::size_t lot_size =
    header->symbol_table_relocations_amount + header->local_relocations_amount;
  void *lot = allocator_(sizeof(std::size_t) * lot_size);
  if (!lot)
  {
    log("LOT allocation failure\n");
    return std::nullopt;
  }

  lot_  = std::span<std::size_t>(static_cast<std::size_t *>(lot), lot_size);
  text_ = parser.get_text();

  if (!process_data(*header, parser))
  {
    return std::nullopt;
  }

  exported_symbols_ = parser.get_exported_symbol_table();

  if (!process_symbol_table_relocations(parser))
  {
    return std::nullopt;
  }
  process_local_relocations(parser);
  process_data_relocations(parser);

  const std::optional<std::size_t> main_address = find_symbol("main");

  if (main_address)
  {
    return Executable{ *main_address, lot_ };
  }

  return std::nullopt;
}

const Header *Loader::process_header(const void *module_address) const
{
  const Header *header = static_cast<const Header *>(module_address);
  if (std::string_view(header->cookie, 4) != "YAFF")
  {
    log("It is not YASIFF file, aborting...\n");
    return nullptr;
  }
  return header;
}

bool Loader::process_data(const Header &header, const Parser &parser)
{
  const auto        data        = parser.get_data();
  const std::size_t data_size   = header.data_length + header.bss_length;

  void             *data_memory = allocator_(data_size);
  if (!data_memory)
  {
    log("Data allocation failure\n");
    return false;
  }

  data_ =
    std::span<std::byte>(static_cast<std::byte *>(data_memory), data_size);

  log(
    "Copying data from: %p, to: %p, size: 0x%x\n",
    data.data(),
    data_memory,
    data_size);
  std::memcpy(data_.data(), data.data(), header.data_length);
  bss_ = data_.subspan(header.data_length, header.bss_length);

  log("Initializing .bss at: %p, size: 0x%x\n", bss_.data(), bss_.size_bytes());
  std::fill(bss_.begin(), bss_.end(), std::byte(0));
  return true;
}

bool Loader::process_symbol_table_relocations(const Parser &parser)
{
  const auto relocations = parser.get_symbol_table_relocations().span();
  log("Processing symbol table relocations: %d\n", relocations.size());
  const auto symbols = parser.get_imported_symbol_table();
  for (const auto &rel : relocations)
  {
    const auto &symbol  = symbols[rel.symbol_index()];
    const auto  address = find_symbol(symbol.name());
    if (!address)
    {
      log("Can't find symbol: %s\n", symbol.name().data());
      return false;
    }
    log("LOT[%d]: 0x%x\n", rel.lot_index(), *address);
    lot_[rel.lot_index()] = *address;
  }

  return true;
}

void Loader::process_local_relocations(const Parser &parser)
{
  const auto relocations = parser.get_local_relocations().span();

  log("Processing local relocations: %d\n", relocations.size());
  for (const auto &rel : relocations)
  {
    const std::size_t relocated_start_address =
      rel.section() == Section::code
        ? reinterpret_cast<std::size_t>(text_.data())
        : reinterpret_cast<std::size_t>(data_.data());
    const std::size_t relocated = relocated_start_address + rel.offset();
    log(
      "| local | lot: %d | base: 0x%lx | offset: 0x%lx | section: %s |\n",
      rel.lot_index(),
      relocated_start_address,
      rel.offset(),
      to_string(rel.section()).data());
    lot_[rel.lot_index()] = relocated;
  }
}

void Loader::process_data_relocations(const Parser &parser)
{
  const auto relocations = parser.get_data_relocations().span();

  log("Processing data relocations: %d\n", relocations.size());
  for (const auto &rel : relocations)
  {
    const std::size_t address_to_change =
      reinterpret_cast<std::size_t>(data_.data()) + rel.to();
    std::size_t *target = reinterpret_cast<std::size_t *>(address_to_change);

    const std::size_t base_address_from =
      rel.section() == Section::data
        ? reinterpret_cast<std::size_t>(data_.data())
        : reinterpret_cast<std::size_t>(text_.data());

    const std::size_t address_from = base_address_from + rel.from();
    log(
      "| data | from: 0x%lx | to: 0x%lx | prev: 0x%lx | section: %s |\n",
      address_from,
      address_to_change,
      *target,
      to_string(rel.section()).data());

    *target = address_from;
  }
}

std::optional<std::size_t> Loader::find_symbol(
  const std::string_view &name) const
{
  log("Searching symbol: %s\n", name.data());

  // Symbols provided by runtime system has highest priority
  if (environment_)
  {
    const auto symbol = environment_->find_symbol(name);
    if (symbol)
    {
      return symbol->address;
    }
  }

  // then local symbols
  if (!exported_symbols_)
  {
    log("Uninitialized exported symbol table\n");
    return std::nullopt;
  }

  for (const auto &symbol : *exported_symbols_)
  {
    printf("Symbol address %p\n", &symbol);
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

  // And symbols imported from other libraries at end
  return std::nullopt;
}

} // namespace yasld
