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

#include <cstdint>

#include <optional>

#include <eul/container/observable/observing_list.hpp>
#include <eul/container/observable/observing_node.hpp>
#include <eul/functional/function.hpp>

#include "yasld/allocator.hpp"
#include "yasld/executable.hpp"
#include "yasld/library.hpp"
#include "yasld/symbol_table.hpp"

#include "yasld/arch.hpp"
namespace yasld
{

class Header;
class Parser;
class Environment;

class Loader
{
public:
  using FileResolverType = eul::function<
    std::optional<const void *>(const std::string_view &filename),
    sizeof(void *)>;
  Loader();
  Loader(const AllocatorType &allocator, const ReleaseType &release);

  void set_environment(const Environment &environment);

  using ObservedExecutable = eul::container::observing_node<Executable>;
  std::optional<ObservedExecutable> load_executable(const void *module_address);
  using ObservedLibrary = eul::container::observing_node<Library>;
  std::optional<ObservedLibrary> load_library(const void *module_address);

  Module *find_module(std::size_t program_counter, bool only_active = false);
  Module *find_module_for_pc_and_lot(std::size_t program_counter, std::size_t lot_address);
  Module *find_module_with_lot(std::size_t lot_address);
  Module *find_active_module(std::size_t program_counter);

  void    register_file_resolver(const FileResolverType &resolver);

private:
  const Header *process_header(const void *module_address) const;
  bool process_data(const Header &header, const Parser &parser, Module &module);
  void process_local_relocations(const Parser &parser, Module &module);
  void process_data_relocations(const Parser &parser, Module &module);
  bool process_symbol_table_relocations(const Parser &parser, Module &module);
  bool load_module(const void *module_address, Module &module);
  std::optional<std::size_t> find_symbol(
    Module                 &module,
    const std::string_view &name) const;
  bool is_fragment_of_module(const Module *module, std::size_t program_counter)
    const;
  FileResolverType   file_resolver_;

  const Environment *environment_;
  // Loaded executables observer
  using ExecutableList = eul::container::observing_list<ObservedExecutable>;
  ExecutableList executables_;
  // Loaded standalone libaries observer,
  // loaded by runtime system not loader as dependency
  using LibrariesList = eul::container::observing_list<ObservedLibrary>;
  LibrariesList libraries_;
};

} // namespace yasld
