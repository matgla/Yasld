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

#include <cstdio>
#include <cstring>
#include <string_view>

#include "yasld/align.hpp"
#include "yasld/header.hpp"
#include "yasld/logger.hpp"
#include "yasld/parser.hpp"
#include "yasld/relocation.hpp"
#include "yasld/section.hpp"
#include "yasld/symbol.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

Loader::Loader(const AllocatorType &allocator, const ReleaseType &release)
  : allocator_(allocator)
  , release_(release)
  , lot_{}
  , text_{}
  , data_{}
  , bss_{}
{
}

std::optional<Executable> Loader::load_executable(
  const void *module_address,
  const Mode  mode)
{
  log("Loading module at address: %p\n", module_address);

  const Header *header = process_header(module_address);
  if (!header)
  {
    return std::nullopt;
  }

  const Parser      parser(header);

  // allocate LOT
  const std::size_t lot_size =
    (header->external_relocations_amount + header->local_relocations_amount);
  const std::size_t lot_size_bytes = lot_size * sizeof(void *);
  log("Allocation of LOT with size: %ld\n", lot_size);

  lot_ = std::span<std::size_t>(
    reinterpret_cast<std::size_t *>(allocator_(lot_size).data()), lot_size);

  if (mode == Mode::copy_only_data)
  {
    text_ = parser.get_text();
  }

  const auto result = process_data(*header, parser);
  process_external_relocations(parser);
  process_local_relocations(parser);
  process_data_relocations(parser);

  exported_symbols_              = parser.get_exported_symbols();
  const std::size_t main_address = find_symbol("main");
  if (main_address != 0)
  {
    return Executable{ main_address,
                       reinterpret_cast<std::size_t>(text_.data()),
                       lot_ };
  }
  return std::nullopt;
}

const Header *Loader::process_header(const void *module_address) const
{
  const Header *header = static_cast<const Header *>(module_address);

  if (std::string_view(header->cookie, 4) != "YAFF")
  {
    log("Not YASIFF file, aborting...\n");
    return nullptr;
  }
  return header;
}

void Loader::process_external_relocations(const Parser &parser)
{
  const auto span = parser.get_external_relocations().span();
  log("Processing external relocations: %d\n", span.size());
  for (const auto &rel : span)
  {
    // TODO(matgla): implement
  }
}

void Loader::process_local_relocations(const Parser &parser)
{
  const auto span = parser.get_local_relocations().span();
  log("Processing local relocations: %d\n", span.size());
  for (const auto &rel : span)
  {
    const uint32_t    lot_index = rel.index() >> 1;
    const auto        section   = static_cast<Section>(rel.index() & 0x01);
    const std::size_t relocated_start_address =
      section == Section::code ? reinterpret_cast<std::size_t>(text_.data())
                               : reinterpret_cast<std::size_t>(data_.data());
    const std::size_t relocated = relocated_start_address + rel.symbol_offset();
    log("Local relocation, lot: %d, new address: 0x%x\n", lot_index, relocated);
    lot_[lot_index] = relocated;
  }
}

void Loader::process_data_relocations(const Parser &parser)
{
  const auto span = parser.get_data_relocations().span();
  log("Processing data relocations: %d\n", span.size());
  for (const auto &rel : span)
  {
    log(
      "Data relocation, index 0x%x, offset: 0x%x, memory: %p\n",
      rel.index(),
      rel.symbol_offset(),
      data_.data());
    std::size_t *to_relocate =
      reinterpret_cast<std::size_t *>(data_.data()) + rel.index();

    std::size_t relocated = reinterpret_cast<std::size_t>(data_.data()) +
                            (*to_relocate - text_.size());
    log(
      "to relocate %p, data offset: 0x%x\n", to_relocate, rel.symbol_offset());
    *to_relocate = relocated;
  }
}

bool Loader::process_data(const Header &header, const Parser &parser)
{
  const auto data = parser.get_data();
  data_           = allocator_(data.size());
  if (data_.empty())
  {
    log("Data memory allocation failure\n");
    return false;
  }

  log(
    "Copying data from %p to %p, size: 0x%x\n",
    data.data(),
    data_.data(),
    data.size());
  std::copy(data.begin(), data.end(), data_.begin());

  bss_ = allocator_(header.bss_length);

  if (bss_.empty())
  {
    log("BSS memory allocation failure\n");
    return false;
  }
  log("BSS initialization on %p, size: 0x%x\n", bss_.data(), bss_.size());
  std::fill(bss_.begin(), bss_.end(), std::byte(0));
  return true;
}

std::size_t Loader::find_symbol(std::string_view name) const
{
  log("Searching symbol: %s\n", name.data());
  for (const auto &symbol : exported_symbols_)
  {
    log("Name: %s\n", symbol.name().data());
    if (symbol.name() == name)
    {
      const std::size_t start_address =
        symbol.section() == Section::code
          ? reinterpret_cast<std::size_t>(text_.data())
          : reinterpret_cast<std::size_t>(data_.data());
      log(
        "Found symbol '%s' at 0x%x\n",
        name.data(),
        start_address + symbol.offset());
      return start_address + symbol.offset();
    }
  }
  return 0;
}

} // namespace yasld
