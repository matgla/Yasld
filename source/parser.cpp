/**
 * parser.cpp
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

#include "yasld/parser.hpp"

#include "yasld/align.hpp"
#include "yasld/header.hpp"
#include "yasld/logger.hpp"

#include "yasld/symbol.hpp"

namespace yasld
{

Parser::Parser(const Header *header)
  : header_{ header }
  , symbol_table_relocation_table_{ align<std::uintptr_t>(
                                      reinterpret_cast<std::uintptr_t>(header) +
                                        sizeof(Header),
                                      header->alignment),
                                    header->symbol_table_relocations_amount }
  , local_relocation_table_{ symbol_table_relocation_table_.address() +
                               symbol_table_relocation_table_.size(),
                             header->local_relocations_amount }
  , data_relocation_table_{ local_relocation_table_.address() +
                              local_relocation_table_.size(),
                            header->data_relocations_amount }
  , imported_symbol_table_{ data_relocation_table_.address() +
                              data_relocation_table_.size(),
                            header->imported_symbols_amount,
                            header->alignment }
  , exported_symbol_table_{ imported_symbol_table_.address() +
                              imported_symbol_table_.size(),
                            header->exported_symbols_amount,
                            header->alignment }
  , text_address_{ align<uintptr_t>(
      exported_symbol_table_.address() + exported_symbol_table_.size(),
      16) }
  , data_address_{ text_address_ + header->code_length }

{
  log("Header starts at       : %p\n", header);
  log(
    "Symbol table relocations at : 0x%lx, size: 0x%lx\n",
    symbol_table_relocation_table_.address(),
    symbol_table_relocation_table_.size());
  log(
    "Local relocations at    : 0x%lx, size: 0x%lx\n",
    local_relocation_table_.address(),
    local_relocation_table_.size());
  log(
    "Data relocations at     : 0x%lx, size: 0x%lx\n",
    data_relocation_table_.address(),
    data_relocation_table_.size());
  log(
    "Imported symbol table at : 0x%lx, size: 0x%lx\n",
    imported_symbol_table_.address(),
    imported_symbol_table_.size());
  log(
    "Exported symbol table at : 0x%lx, size: 0x%lx\n",
    exported_symbol_table_.address(),
    exported_symbol_table_.size());
  log(
    "Text section at : 0x%lx, size: 0x%lx\n",
    text_address_,
    header->code_length);
  log(
    "Data section at : 0x%lx, size: 0x%lx\n",
    data_address_,
    header->data_length);

  log("Exported symbol table\n");
  for (const auto &e : exported_symbol_table_)
  {
    log("%s\n", e.name().data());
  }
}

const SymbolTable Parser::get_exported_symbol_table() const
{
  return exported_symbol_table_;
}

const SymbolTable Parser::get_imported_symbol_table() const
{
  return imported_symbol_table_;
}

const RelocationTable<LocalRelocation> Parser::get_local_relocations() const
{
  return local_relocation_table_;
}

const RelocationTable<DataRelocation> Parser::get_data_relocations() const
{
  return data_relocation_table_;
}

const RelocationTable<Relocation> Parser::get_symbol_table_relocations() const
{
  return symbol_table_relocation_table_;
}

std::span<const std::byte> Parser::get_data() const
{
  return std::span<const std::byte>(
    reinterpret_cast<const std::byte *>(data_address_), header_->data_length);
}

std::span<const std::byte> Parser::get_text() const
{
  return std::span<const std::byte>(
    reinterpret_cast<const std::byte *>(text_address_), header_->code_length);
}

} // namespace yasld
