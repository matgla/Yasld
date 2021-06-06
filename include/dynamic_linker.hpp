// This file is part of MSOS project. This is simple OS for embedded development devices.
// Copyright (C) 2019 Mateusz Stadnik
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <string_view>
#include <vector>

#include <eul/error/error_code.hpp>
#include <eul/error/error_category.hpp>

#include "msos/dynamic_linker/fwd.hpp"
#include "msos/dynamic_linker/loaded_module.hpp"

namespace msos
{
namespace dl
{

constexpr int LoadingModeCopyData = 0x01;
constexpr int LoadingModeCopyText = 0x02;

class DynamicLinker
{
public:
    DynamicLinker();
    void unload_module(const LoadedModule* module);

    template <typename Environment>
    const LoadedModule* load_module(const std::size_t* module_address, const int mode, const Environment& environment, eul::error::error_code& ec)
    {
        return load_module(module_address, mode, reinterpret_cast<const SymbolEntry*>(environment.data().data()), environment.data().size(), ec);
    }

    const LoadedModule* load_module(const std::size_t* module_address, const int mode, const SymbolEntry* entries, std::size_t number_of_entries, eul::error::error_code& ec);
    const Symbol* find_symbol(const std::size_t address, const uint32_t number_of_symbols, uint32_t symbol_code);
    std::size_t get_lot_for_module_at(std::size_t address);


private:

    uint32_t get_size_of_lot(const ModuleHeader& header);
    bool allocate_lot(LoadedModule& loaded_module);

    void process_local_relocations(std::size_t local_relocations_address, LoadedModule& loaded_module);

    template <typename Environment>
    bool process_external_relocations(std::size_t external_relocations_address, const Environment& env, LoadedModule& loaded_module)
    {
        return process_external_relocations(external_relocations_address, reinterpret_cast<const SymbolEntry*>(env.data().data()), env.data().size());
    }

    const SymbolEntry* find_symbol(const SymbolEntry* entries, std::size_t number_of_entries, uint32_t symbol_code);

    bool process_external_relocations(std::size_t external_relocations_address, const SymbolEntry* entries, std::size_t number_of_entries, LoadedModule& loaded_module);
    void process_data_relocations(std::size_t data_relocations_address, LoadedModule& loaded_module);

    std::size_t get_relocations_size(const ModuleHeader& header);
    std::size_t get_symbols_size(const std::size_t address, const uint32_t number_of_symbols);


private:
    std::vector<LoadedModule> modules_;
};

} // namespace dl
} // namespace msos

