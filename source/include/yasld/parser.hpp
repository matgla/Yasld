/**
 * parser.hpp
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

#include <cstdint>
#include <span>
#include <string_view>

#include "yasld/data_relocation.hpp"
#include "yasld/dependency_list.hpp"
#include "yasld/local_relocation.hpp"
#include "yasld/relocation.hpp"
#include "yasld/relocation_table.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

class Header;

class Parser
{
public:
  Parser(const Header *header);

  const SymbolTable                      get_exported_symbol_table() const;
  const SymbolTable                      get_imported_symbol_table() const;

  const RelocationTable<LocalRelocation> get_local_relocations() const;
  const RelocationTable<DataRelocation>  get_data_relocations() const;
  const RelocationTable<Relocation>      get_symbol_table_relocations() const;

  std::span<const std::byte>             get_data() const;
  std::span<const std::size_t>           get_init() const;
  std::span<const std::byte>             get_text() const;

  const std::string_view                &name() const;

  const DependencyList                  &get_imported_libraries() const;

private:
  const Header                          *header_;

  std::string_view                       name_;
  const DependencyList                   imported_libaries_;
  const RelocationTable<Relocation>      symbol_table_relocation_table_;
  const RelocationTable<LocalRelocation> local_relocation_table_;
  const RelocationTable<DataRelocation>  data_relocation_table_;

  const SymbolTable                      imported_symbol_table_;
  const SymbolTable                      exported_symbol_table_;

  const uintptr_t                        text_address_;
  const uintptr_t                        init_address_;
  const uintptr_t                        data_address_;
};

} // namespace yasld
