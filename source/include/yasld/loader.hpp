/**
 * loader.hpp
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

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include <eul/functional/function.hpp>

#include "yasld/executable.hpp"
#include "yasld/symbol_table.hpp"
namespace yasld
{

class Header;
class Parser;

class Loader
{
public:
  using AllocatorType =
    eul::function<std::span<std::byte>(std::size_t size), sizeof(void *)>;
  using ReleaseType = eul::function<void(std::span<std::byte>), sizeof(void *)>;

  Loader(const AllocatorType &allocator, const ReleaseType &release);

  enum class Mode : uint8_t
  {
    copy_code_and_data,
    copy_only_data,
    no_copy
  };

  std::optional<Executable> load_executable(
    const void *module_address,
    const Mode  mode);

  std::size_t find_symbol(std::string_view name) const;

private:
  const Header *process_header(const void *module_address) const;
  bool          process_data(const Header &header, const Parser &parser);
  void          process_external_relocations(const Parser &parser);
  void          process_local_relocations(const Parser &parser);
  void          process_data_relocations(const Parser &parser);

  AllocatorType allocator_;
  ReleaseType   release_;

  std::span<std::size_t>     lot_;
  std::span<const std::byte> text_;
  std::span<std::byte>       data_;
  std::span<std::byte>       bss_;
  SymbolTable                exported_symbols_;
};

} // namespace yasld
