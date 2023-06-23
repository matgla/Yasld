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

#include "yasld/parser.hpp"

#include "yasld/align.hpp"

namespace yasld
{
Parser::Parser(const Header *header)
  : header_{ header }
  , exported_relocations_{ reinterpret_cast<std::uintptr_t>(header) +
                             sizeof(header),
                           header->exported_relocations_amount }
  , external_relocations_{ exported_relocations_.address() +
                             exported_relocations_.size(),
                           header->external_relocations_amount }
  , local_relocations_{ external_relocations_.address() +
                          external_relocations_.size(),
                        header->local_relocations_amount }
  , data_relocations_{ local_relocations_.address() + data_relocations_.size(),
                       header->data_relocations_amount }
  , exported_symbols_{ data_relocations_.address() + data_relocations_.size(),
                       header->exported_symbols_amount }
  , external_symbols_{ exported_symbols_.address() + exported_symbols_.size(),
                       header->external_symbols_amount }
  , text_address_{ align<std::uintptr_t, 16>(
      external_symbols_.address() + external_symbols_.size()) }
  , data_address_{ text_address_ + header->code_length }
{
}

const SymbolTable Parser::get_exported_symbols() const
{
  return exported_symbols_;
}

const SymbolTable Parser::get_external_symbols() const
{
  return external_symbols_;
}

const RelocationTable Parser::get_exported_relocations() const
{
  return exported_relocations_;
}

const RelocationTable Parser::get_local_relocations() const
{
  return local_relocations_;
}

const RelocationTable Parser::get_data_relocations() const
{
  return data_relocations_;
}

const RelocationTable Parser::get_external_relocations() const
{
  return external_relocations_;
}

std::span<const uint8_t> Parser::get_text() const
{
  return std::span<const uint8_t>(
    reinterpret_cast<const uint8_t *>(text_address_), header_->code_length);
}

std::span<const uint8_t> Parser::get_data() const
{
  return std::span<const uint8_t>(
    reinterpret_cast<const uint8_t *>(data_address_), header_->data_length);
}

} // namespace yasld
