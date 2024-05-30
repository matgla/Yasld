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

namespace yasld
{

Parser::Parser(const Header *header)
  : header_{ header }
  , name_{ reinterpret_cast<const char *>(header) + sizeof(Header) }
  , imported_libaries_{ align<std::uintptr_t>(
                          reinterpret_cast<std::uintptr_t>(header) +
                            sizeof(Header) + name_.size(),
                          header->alignment),
                        header->external_libraries_amount,
                        header->alignment }
  , symbol_table_relocation_table_{ imported_libaries_.address() +
                                      imported_libaries_.size(),
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
  , init_address_{ text_address_ + header->code_length }
  , data_address_{ init_address_ + header->init_length }

{
  log("Module name: %s\n", name_.data());
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
    "Init section at: 0x%lx, size: 0x%lx\n",
    init_address_,
    header->init_length);
  log(
    "Data section at : 0x%lx, size: 0x%lx\n",
    data_address_,
    header->data_length);

  log("Exported symbol table\n");
  for (const auto &e : exported_symbol_table_)
  {
    log("%s\n", e.name().data());
  }

  log("Imported libraries (%d):\n", header->external_libraries_amount);
  for (const auto &l : imported_libaries_)
  {
    log("  %s\n", l.name().data());
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

std::span<const std::size_t> Parser::get_init() const
{
  return std::span<const std::size_t>(
    reinterpret_cast<const std::size_t *>(init_address_),
    header_->init_length / sizeof(std::size_t));
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

const std::string_view &Parser::name() const
{
  return name_;
}

const DependencyList &Parser::get_imported_libraries() const
{
  return imported_libaries_;
}

} // namespace yasld
