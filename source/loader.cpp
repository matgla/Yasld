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
#include <inttypes.h>
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

  log("Alignment is: %d\n", header->alignment);
  const Parser      parser(header);

  // allocate LOT
  const std::size_t lot_size =
    (header->external_relocations_amount + header->local_relocations_amount);
  const std::size_t lot_size_bytes = lot_size * sizeof(void *);
  log("Allocation of LOT with size: %ldB\n", lot_size_bytes);

  lot_ = std::span<std::size_t>(
    reinterpret_cast<std::size_t *>(allocator_(lot_size_bytes).data()),
    lot_size);

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
  log("Processing external relocations: %zd\n", span.size());
  for (const auto &rel : span)
  {
    // TODO(matgla): implement
  }
}

void Loader::process_local_relocations(const Parser &parser)
{
  const auto span = parser.get_local_relocations().span();
  log("Processing local relocations: %zd\n", span.size());
  for (const auto &rel : span)
  {
    const std::size_t relocated_start_address =
      rel.section() == Section::code
        ? reinterpret_cast<std::size_t>(text_.data())
        : reinterpret_cast<std::size_t>(data_.data());
    const std::size_t relocated = relocated_start_address + rel.offset();
    log(
      "[relocation] | local | lot: %4d | new address: 0x%-16zx | %s |\n",
      rel.lot_index(),
      relocated,
      to_string(rel.section()).data());
    lot_[rel.lot_index()] = relocated;
  }
}

void Loader::process_data_relocations(const Parser &parser)
{
  const auto span = parser.get_data_relocations().span();
  log("Processing data relocations: %zd\n", span.size());
  for (const auto &rel : span)
  {
    std::size_t relocate =
      reinterpret_cast<std::size_t>(data_.data()) + rel.to_offset();
    std::size_t *to_relocate = reinterpret_cast<std::size_t *>(relocate);

    std::size_t  relocated   = reinterpret_cast<std::size_t>(data_.data()) +
                            (*to_relocate - text_.size_bytes());
    log(
      "to relocate %p, data offset: 0x%x, relocated: 0x%x\n",
      (*to_relocate),
      (*to_relocate - text_.size_bytes()),
      relocated);

    std::size_t start_address = rel.section() == Section::data
                                  ? reinterpret_cast<std::size_t>(data_.data())
                                  : reinterpret_cast<std::size_t>(text_.data());

    std::size_t should_be     = start_address + rel.from_offset();
    *to_relocate              = should_be;
  }
}

bool Loader::process_data(const Header &header, const Parser &parser)
{
  const auto data = parser.get_data();
  data_           = allocator_(header.data_length + header.bss_length);
  if (data_.empty())
  {
    log("Data memory allocation failure\n");
    return false;
  }

  log(
    "Copying data from %p to %p, size: 0x%zx\n",
    data.data(),
    data_.data(),
    data.size_bytes());

  std::memcpy(data_.data(), data.data(), data.size_bytes());

  bss_ = data_.subspan(header.data_length, header.bss_length);

  if (bss_.empty())
  {
    log("BSS memory allocation failure\n");
    return false;
  }
  log("BSS initialization on %p, size: 0x%x\n", bss_.data(), bss_.size_bytes());
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
