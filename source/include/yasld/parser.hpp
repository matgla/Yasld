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

#include "yasld/header.hpp"
#include "yasld/relocation.hpp"
#include "yasld/relocation_table.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

class Parser
{
public:
  Parser(const Header *header);

  const SymbolTable                      get_exported_symbols() const;
  const SymbolTable                      get_external_symbols() const;

  const RelocationTable<LocalRelocation> get_local_relocations() const;
  const RelocationTable<DataRelocation>  get_data_relocations() const;
  const RelocationTable<Relocation>      get_external_relocations() const;

  std::span<const std::byte>             get_data() const;
  std::span<const std::byte>             get_text() const;

private:
  const Header                          *header_;

  const RelocationTable<Relocation>      external_relocations_;
  const RelocationTable<LocalRelocation> local_relocations_;
  const RelocationTable<DataRelocation>  data_relocations_;
  const SymbolTable                      exported_symbols_;
  const SymbolTable                      external_symbols_;
  const std::uintptr_t                   text_address_;
  const std::uintptr_t                   data_address_;
};

} // namespace yasld
