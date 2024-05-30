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

#include <cstring>

#include "yasld/environment.hpp"
#include "yasld/header.hpp"
#include "yasld/logger.hpp"
#include "yasld/parser.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

Loader::Loader(const AllocatorType &allocator, const ReleaseType &release)
  : environment_{ nullptr }
{
  YasldAllocatorHolder::get().set_allocator(allocator);
  YasldAllocatorHolder::get().set_release(release);
}

Loader::Loader()
  : environment_{ nullptr }
{
}

void Loader::set_environment(const Environment &environment)
{
  environment_ = &environment;
}

bool Loader::load_module(const void *module_address, Module &module)
{
  log("Loading module from address: %p\n", module_address);

  const Header *header = process_header(module_address);
  if (!header)
  {
    return false;
  }

  const Parser      parser(header);
  const std::size_t lot_size =
    header->symbol_table_relocations_amount + header->local_relocations_amount;

  module.set_name(parser.name());

  log("Allocation of LOT with size: %d\n", lot_size);
  if (!module.allocate_lot(lot_size))
  {
    log("LOT allocation failure\n");
    return false;
  }

  log(
    "LOT allocated at %p with %d entries.\n",
    module.get_lot().data(),
    module.get_lot().size());

  module.set_text(parser.get_text());
  module.relocate_init(parser.get_init());

  // import modules
  if (header->external_libraries_amount)
  {
    if (!module.allocate_modules(header->external_libraries_amount))
    {
      log("Modules allocation failed\n");
      return false;
    }
    if (!file_resolver_)
    {
      log("Module has imported libraries, but file resolver not "
          "set!\n");
      return false;
    }

    auto &modules = module.get_modules();
    for (const auto &dependency : parser.get_imported_libraries())
    {
      const auto address = file_resolver_(dependency.name());
      if (!address)
      {
        return false;
      }
      const yasld::Header *dependent_header =
        reinterpret_cast<const yasld::Header *>(*address);
      if (std::string_view(dependent_header->cookie, 4) != "YAFF")
      {
        log("Module %s is not YAFF file\n", dependency.name().data());
        return false;
      }
      if (dependent_header->type == Header::Type::Executable)
      {
        modules.emplace_back(
          static_cast<Module *>(YasldAllocatorHolder::get().get_allocator()(
            sizeof(Executable), AllocationType::Module)),
          YasldDeleter<Module>());
        new (modules.back().get()) Executable;
      }
      else if (dependent_header->type == Header::Type::Library)
      {
        modules.emplace_back(
          static_cast<Module *>(YasldAllocatorHolder::get().get_allocator()(
            sizeof(Library), AllocationType::Module)),
          YasldDeleter<Module>());
        new (modules.back().get()) Library;
      }
      else
      {
        log("Unknown module type for: %s\n", dependency.name().data());
        return false;
      }

      if (!load_module(*address, *modules.back().get()))
      {
        return false;
      }
    }
  }

  if (!process_data(*header, parser, module))
  {
    return false;
  }

  module.set_exported_symbol_table(parser.get_exported_symbol_table());

  if (!process_symbol_table_relocations(parser, module))
  {
    return false;
  }
  process_local_relocations(parser, module);
  process_data_relocations(parser, module);

  return true;
}

std::optional<Loader::ObservedExecutable> Loader::load_executable(
  const void *module_address)
{
  ObservedExecutable executable;
  if (!load_module(module_address, *executable))
  {
    return std::nullopt;
  }

  if (!executable->initialize_main())
  {
    return std::nullopt;
  }

  executables_.push_back(executable);
  return executable;
}

std::optional<Loader::ObservedLibrary> Loader::load_library(
  const void *module_address)
{
  ObservedLibrary library;
  if (!load_module(module_address, *library))
  {
    return std::nullopt;
  }

  libraries_.push_back(library);
  return library;
}

const Header *Loader::process_header(const void *module_address) const
{
  const Header *header = static_cast<const Header *>(module_address);
  if (std::string_view(header->cookie, 4) != "YAFF")
  {
    log("It is not YASIFF file, aborting...\n");
    return nullptr;
  }
  return header;
}

bool Loader::process_data(
  const Header &header,
  const Parser &parser,
  Module       &module)
{
  const auto        data_initializer = parser.get_data();
  const std::size_t data_size        = header.data_length + header.bss_length;

  if (!module.allocate_data(header.data_length, header.bss_length))
  {
    log("Data allocation failure\n");
    return false;
  }

  log(
    "Copying data from: %p, to: %p, size: 0x%x\n",
    data_initializer.data(),
    module.get_data().data(),
    data_size);

  std::memcpy(
    module.get_data().data(), data_initializer.data(), header.data_length);

  log(
    "Initializing .bss at: %p, size: 0x%x\n",
    module.get_bss().data(),
    module.get_bss().size_bytes());
  std::fill(module.get_bss().begin(), module.get_bss().end(), std::byte(0));
  return true;
}

bool Loader::process_symbol_table_relocations(
  const Parser &parser,
  Module       &module)
{
  const auto relocations = parser.get_symbol_table_relocations().span();
  log("Processing symbol table relocations: %d\n", relocations.size());
  const auto symbols = parser.get_imported_symbol_table();
  for (const auto &rel : relocations)
  {
    const auto &symbol  = symbols[rel.symbol_index()];
    const auto  address = find_symbol(module, symbol.name());
    if (!address)
    {
      log("Can't find symbol: %s\n", symbol.name().data());
      return false;
    }
    log("LOT[%d]: 0x%x\n", rel.lot_index(), *address);
    module.get_lot()[rel.lot_index()] = *address;
  }

  return true;
}

void Loader::process_local_relocations(const Parser &parser, Module &module)
{
  const auto relocations = parser.get_local_relocations().span();

  log("Processing local relocations: %d\n", relocations.size());
  for (const auto &rel : relocations)
  {
    std::size_t relocated_start_address =
      get_base_address(rel.section(), module);
    const std::size_t relocated = relocated_start_address + rel.offset();
    log(
      "| local | lot: %d | base: 0x%lx | offset: 0x%lx | section: %s |\n",
      rel.lot_index(),
      relocated_start_address,
      rel.offset(),
      to_string(rel.section()).data());
    module.get_lot()[rel.lot_index()] = relocated;
  }
}

std::size_t Loader::get_base_address(Section section, Module &module)
{
  switch (section)
  {
  case Section::code:
    return reinterpret_cast<std::size_t>(module.get_text().data());
  case Section::data:
    return reinterpret_cast<std::size_t>(module.get_data().data());
  case Section::init:
    return reinterpret_cast<std::size_t>(module.get_init().data());
  case Section::unknown:
    // report error
    return 0;
  }
  // report error
  return 0;
}

void Loader::process_data_relocations(const Parser &parser, Module &module)
{
  const auto relocations = parser.get_data_relocations().span();

  log("Processing data relocations: %d\n", relocations.size());
  for (const auto &rel : relocations)
  {
    const std::size_t address_to_change =
      reinterpret_cast<std::size_t>(module.get_data().data()) + rel.to();
    std::size_t *target = reinterpret_cast<std::size_t *>(address_to_change);

    const std::size_t base_address_from =
      get_base_address(rel.section(), module);

    const std::size_t address_from = base_address_from + rel.from();
    log(
      "| data | from: 0x%lx | to: 0x%lx | prev: 0x%lx | section: %s |\n",
      address_from,
      address_to_change,
      *target,
      to_string(rel.section()).data());

    *target = address_from;
  }
}

std::optional<std::size_t> Loader::find_symbol(
  Module                 &module,
  const std::string_view &name) const
{
  log("Searching symbol: %s\n", name.data());

  // Symbols provided by runtime system has highest priority
  if (environment_)
  {
    const auto symbol = environment_->find_symbol(name);
    if (symbol)
    {
      return symbol->address;
    }
  }

  // Followed by symbols imported from other libraries
  const auto symbol = module.find_symbol(name);
  if (symbol)
  {
    return *symbol;
  }
  // And own symbols at end
  return std::nullopt;
}

Module *Loader::find_active_module(std::size_t program_counter)
{
  for (auto &e : executables_)
  {
    auto ptr = e->find_active_module_for_program_counter(program_counter);
    if (ptr)
    {
      return *ptr;
    }
  }

  for (auto &l : libraries_)
  {
    auto ptr = l->find_active_module_for_program_counter(program_counter);
    if (ptr)
    {
      return *ptr;
    }
  }

  return nullptr;
}

Module *Loader::find_module_with_lot(std::size_t lot_address)
{
  for (auto &e : executables_)
  {
    auto ptr = e->find_module_with_lot(lot_address);
    if (ptr)
    {
      return *ptr;
    }
  }
  for (auto &l : libraries_)
  {
    auto ptr = l->find_module_with_lot(lot_address);
    if (ptr)
    {
      return *ptr;
    }
  }
  return nullptr;
}

Module *Loader::find_module_for_pc_and_lot(
  std::size_t program_counter,
  std::size_t lot_address)
{
  Module *parent = find_module_with_lot(lot_address);
  // if has no parent it was called from runtime system
  if (parent == nullptr)
  {
    for (auto &module : executables_)
    {
      if (module->is_module_for_program_counter(program_counter))
      {
        return &(*module);
      }
    }
    for (auto &module : libraries_)
    {
      if (module->is_module_for_program_counter(program_counter))
      {
        return &(*module);
      }
    }
  }

  auto module = parent->find_module_for_program_counter(program_counter);
  if (module)
  {
    return *module;
  }

  return nullptr;
}

void Loader::register_file_resolver(const FileResolverType &resolver)
{
  file_resolver_ = resolver;
}

} // namespace yasld
