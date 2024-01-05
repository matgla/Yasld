/**
 * module.hpp
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

#include <memory>
#include <optional>
#include <span>
#include <string_view>
#include <vector>

#include <eul/functional/function.hpp>

#include "yasld/allocator.hpp"
#include "yasld/arch.hpp"
#include "yasld/symbol_table.hpp"

namespace yasld
{

// Base for executable and library
class Module
{
public:
  virtual ~Module()      = default;

  Module(const Module &) = delete;
  Module(Module &&)      = default;
  Module();

  bool allocate_lot(std::size_t size);
  bool allocate_data(std::size_t data_size, std::size_t bss_size);
  bool allocate_modules(std::size_t number_of_modules);

  void set_text(const std::span<const std::byte> &text);
  void set_exported_symbol_table(const SymbolTable &table);

  std::span<std::size_t>            get_lot();
  std::span<const std::byte>        get_text() const;
  std::span<std::byte>              get_data();
  std::span<std::byte>              get_bss();
  std::span<const std::byte>        get_data() const;
  std::span<const std::byte>        get_bss() const;
  const std::optional<SymbolTable> &get_exported_symbol_table() const;
  std::optional<std::size_t> find_symbol(const std::string_view &name) const;

  // If call from foreign module previous R9 and PC counter inside wrapper must
  // be saved
  // Works thanks to limitation that shared libraries cannot contain
  // cyclic dependency
  void                       save_caller_state(ForeignCallContext ctx);
  ForeignCallContext         restore_caller_state();

  using ModuleHolder = std::unique_ptr<Module, YasldDeleter<Module>>;
  using ModulesContainer =
    std::vector<ModuleHolder, YasldAllocator<ModuleHolder>>;

  ModulesContainer       &get_modules();

  void                    set_name(const std::string_view &name);
  const std::string_view &get_name() const;

  bool                    is_module_for_program_counter(
                       std::size_t program_counter);

  std::optional<Module *> find_module_for_program_counter(
    std::size_t program_counter);

  std::optional<Module *> find_active_module_for_program_counter(
    std::size_t program_counter);

  std::optional<Module *> find_module_with_lot(std::size_t lot_address);

  bool get_active() const;
  void set_active(bool active);

protected:
  std::optional<Module *> find_module_for_program_counter_impl(
    std::size_t program_counter,
    bool        only_active = false);

  bool                    is_module_for_program_counter_impl(
                       std::size_t program_counter,
                       bool only_active = false);

  std::vector<std::size_t, YasldAllocator<std::size_t>> lot_;
  std::vector<std::byte>                                data_memory_;
  std::span<const std::byte>                            text_;
  std::span<std::byte>                                  data_;
  std::span<std::byte>                                  bss_;
  std::optional<SymbolTable>                            exported_symbols_;
  ForeignCallContext                                    foreign_call_context_;
  ModulesContainer                                      imported_modules_;
  std::string_view                                      name_;
  // In single threaded only one instance of library is active
  // for multithreaded it has to be connected to thread info,
  // thread info may point currently used module, to determine which
  // tree needs to be searched
  bool                                                  active_;
};

} // namespace yasld
