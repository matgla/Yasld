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

extern "C"
{
  int call_main(int argc, char *argv[], std::size_t address, void *lot);
}

namespace yasld
{

Loader::Loader(
  std::span<std::size_t> memory_for_lot,
  std::span<std::byte>   memory_for_app)
  : memory_for_lot_(memory_for_lot)
  , memory_for_app_(memory_for_app)
{
}

std::optional<Executable> Loader::load_executable(const void *module_address)
{
  log("Loading module at address: %p\n", module_address);
  log("Memory for app: %p\n", memory_for_app_.data());
  log("Memory for lot: %p\n", memory_for_lot_.data());

  const Header *header = process_header(module_address);
  if (!header)
  {
    return std::nullopt;
  }

  const Parser      parser(header);

  // allocate LOT
  const std::size_t lot_size =
    header->external_relocations_amount + header->local_relocations_amount;
  log("Allocation of LOT with size: %ld\n", lot_size * sizeof(std::size_t));

  log("Code is located at: %p\n", parser.get_text().data());

  const auto data = parser.get_data();
  log(
    "Copying data from %p to %p, size: 0x%lx\n",
    data.data(),
    memory_for_app_.data(),
    data.size());

  // std::memcpy(memory_for_app_.data(), data, header->data_length);
  // std::memset(
  // memory_for_app_.data() + header->data_length, 0, header->bss_length);
  //
  // log("Ram memory usage: 0x%x\n", header->data_length + header->bss_length);
  //
  // Process external relocations
  // log(
  // "Processing external relocations: %d\n",
  // header->external_relocations_amount);
  // for (int i = 0; i < header->external_relocations_amount; ++i)
  // {
  // TODO(matgla): implement
  // }
  //
  // log("Processing local relocations: %d\n",
  // header->local_relocations_amount);
  //
  // for (int i = 0; i < header->local_relocations_amount; ++i)
  // {
  // const Relocation *relocation = reinterpret_cast<const Relocation *>(
  // address + local_relocations_offset + i * sizeof(Relocation));
  //
  // const uint32_t lot_index = relocation->index() >> 1;
  // const auto     section   = static_cast<Section>(relocation->index() &
  // 0x01); if (section == Section::code)
  // {
  // const std::size_t relocated =
  // address + code_offset + relocation->symbol_offset();
  // log("Local relocation lot: %d, addr: 0x%x\n", lot_index, relocated);
  // memory_for_lot_[lot_index] = relocated;
  // }
  // else if (section == Section::data)
  // {
  // log("Data relocation offset: 0x%x\n", relocation->symbol_offset());
  // const std::size_t relocated =
  // reinterpret_cast<std::size_t>(memory_for_app_.data()) +
  // relocation->symbol_offset();
  // memory_for_lot_[lot_index] = relocated;
  // log("Local relocation lot: %d, addr: 0x%x\n", lot_index, relocated);
  // }
  // }
  //
  // log("Processing data relocations: %d\n", header->data_relocations_amount);
  // for (int i = 0; i < header->data_relocations_amount; ++i)
  // {
  // const Relocation *relocation = reinterpret_cast<const Relocation *>(
  // address + data_relocations_offset + i * sizeof(Relocation));
  // printf(
  // "Relocation index 0x%x, symbol offset 0x%x, memory address: %p\n",
  // relocation->index(),
  // relocation->symbol_offset(),
  // memory_for_app_.data());
  //
  // std::size_t *to_relocate =
  // reinterpret_cast<std::size_t *>(memory_for_app_.data()) +
  // relocation->index();
  //
  // if (relocation->index() < 3)
  //   {
  //     std::size_t relocated =
  //       reinterpret_cast<std::size_t>(memory_for_app_.data()) +
  //       relocation->symbol_offset();
  //     printf(
  //       "to relocate 0x%x, data offset: 0x%x\n",
  //       (*to_relocate),
  //       header->code_length);

  //     log("Data reloc, at %p to 0x%x\n", to_relocate, relocated);
  //     *to_relocate = relocated;
  //   }
  //   else
  //   {
  //     std::size_t relocated =
  //       reinterpret_cast<std::size_t>(memory_for_app_.data()) +
  //       (*to_relocate - header->code_length);
  //     printf(
  //       "to relocate 0x%x, data offset: 0x%x\n",
  //       (*to_relocate),
  //       header->code_length);
  //
  //     log("Data reloc, at %p to 0x%x\n", to_relocate, relocated);
  //     *to_relocate = relocated;
  //     // }
  //   }
  //
  //   log(
  //     "%s offset: 0x%x\n",
  //     symbol_table_root->name().data(),
  //     symbol_table_root->offset());
  //   const std::size_t main = address + code_offset +
  //   symbol_table_root->offset(); log("Calling main at: 0x%x\n", main); int
  //   argc   = 1; char *argv[] = { { "appname" } }; call_main(argc, argv, main,
  //   memory_for_lot_.data());
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

} // namespace yasld
