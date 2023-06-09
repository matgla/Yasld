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
#include <string_view>

#include "yasld/header.hpp"
#include "yasld/relocation.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

Loader::Loader(
  std::span<std::byte> memory_for_lot,
  std::span<std::byte> memory_for_app)
  : memory_for_lot_(memory_for_lot)
  , memory_for_app_(memory_for_app)
{
}

void Loader::load_module(const void *module_address)
{
  printf("Loading module at address: %p\n", module_address);

  const Header *header = static_cast<const Header *>(module_address);

  if (std::string_view(header->cookie, 4) != "YAFF")
  {
    printf("Not YASIFF file, aborting...\n");
    return;
  }

  std::uintptr_t    address = static_cast<std::uintptr_t>(module_address);

  const std::size_t relocations_amount =
    header->external_relocations_amount + header->local_relocations_amount +
    header->data_relocations_amount + header->exported_relocations_amount;
  const Symbol *symbol_table_root =
    module_address + sizeof(Header) + relocations_amount * sizeof(Relocation);

  const SymbolTable exported_symbol_table(
    header->exported_symbols_amount, symbol_table_root);

  printf("Symbol table size: %d\n", exported_symbol_table.size());
}

} // namespace yasld
