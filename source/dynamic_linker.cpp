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

#include "msos/dynamic_linker/dynamic_linker.hpp"

#include <cstring>

#include "msos/dynamic_linker/dynamic_linker_errors.hpp"
#include "msos/dynamic_linker/environment.hpp"
#include "msos/dynamic_linker/module.hpp"
#include "msos/dynamic_linker/module_header.hpp"
#include "msos/dynamic_linker/relocation.hpp"
#include "msos/dynamic_linker/symbol.hpp"

namespace msos
{
namespace dl
{

DynamicLinker::DynamicLinker()
    : modules_{}
{
}

void DynamicLinker::unload_module(const LoadedModule* module)
{
    auto module_it = std::find_if(modules_.begin(), modules_.end(), [module](const auto& loaded_module) {
        return &loaded_module == module;
    });
    if (module_it != modules_.end())
    {
        modules_.erase(module_it);
    }
}

const LoadedModule* DynamicLinker::load_module(const std::size_t* module_address, const int mode, const SymbolEntry* entries, std::size_t number_of_entries, eul::error::error_code& ec)
{
    const ModuleHeader& header = *reinterpret_cast<const ModuleHeader*>(module_address);

    if (header.cookie() != "MSDL")
    {
        ec = DynamicLinkerErrors::CookieValidationFailure;
        return nullptr;
    }

    const std::size_t relocation_section_address = reinterpret_cast<std::size_t>(module_address) + header.size();
    const std::size_t relocation_section_size = get_relocations_size(header);
    const std::size_t symbol_section_address = relocation_section_address + relocation_section_size;
    const std::size_t symbol_section_size = get_symbols_size(symbol_section_address, header.number_of_external_symbols() + header.number_of_exported_symbols());

    const std::size_t not_aligned_code_address = symbol_section_address + symbol_section_size;
    const std::size_t code_address = not_aligned_code_address % 16 ? not_aligned_code_address + (16 - (not_aligned_code_address % 16)) : not_aligned_code_address;
    const std::size_t data_address = code_address + header.code_size();

    const auto* main = find_symbol(symbol_section_address, header.number_of_external_symbols() + header.number_of_exported_symbols(), 1);
    modules_.emplace_back(header);
    LoadedModule& loaded_module = modules_.back();
    Module& module = loaded_module.get_module();

    if (mode & LoadingModeCopyText)
    {
        if (!module.allocate_text())
        {
            ec = DynamicLinkerErrors::TextAllocationFailure;
            return nullptr;
        }
        std::memcpy(module.get_text().data(), reinterpret_cast<const uint8_t*>(code_address), header.code_size());
    }
    else
    {
        module.set_text(std::span(reinterpret_cast<uint8_t*>(code_address), header.code_size()));
    }

    if (!module.allocate_data())
    {
        ec = DynamicLinkerErrors::DataAllocationFailure;
        return nullptr;
    }

    std::memcpy(module.get_data().data(), reinterpret_cast<const uint8_t*>(data_address), header.data_size());
    std::memset(module.get_data().data() + header.data_size(), 0, header.bss_size());

    if (main)
    {
        const std::size_t main_function_address = (reinterpret_cast<std::size_t>(module.get_text().data()) + main->offset());
        loaded_module.set_start_address(main_function_address);
    }

    if (!allocate_lot(loaded_module))
    {
        ec = DynamicLinkerErrors::LotAllocationFailure;
        return nullptr;
    }

    const std::size_t external_relocations_address = relocation_section_address;
    if(!process_external_relocations(external_relocations_address, entries, number_of_entries, loaded_module))
    {
        ec = DynamicLinkerErrors::ExternalRelocationFailure;
        return nullptr;
    }

    const std::size_t local_relocations_address = external_relocations_address + sizeof(Relocation) * header.number_of_external_relocations();
    process_local_relocations(local_relocations_address, loaded_module);

    // const std::size_t data_relocations_address = local_relocations_address + sizeof(Relocation) * header.number_of_local_relocations();
    process_data_relocations(local_relocations_address, loaded_module); // why local relocations passed here? anyway otherwise not working, but whole code will be reimplemented

    return &loaded_module;
}

const Symbol* DynamicLinker::find_symbol(const std::size_t address, const uint32_t number_of_symbols, uint32_t symbol_code)
{
    const Symbol* symbol = reinterpret_cast<const Symbol*>(address);
    for (uint32_t i = 0; i < number_of_symbols; ++i)
    {
        if (symbol->code() == symbol_code)
        {
            return symbol;
        }
        symbol = &symbol->next();
    }
    return nullptr;
}

std::size_t DynamicLinker::get_lot_for_module_at(std::size_t address)
{
    for (const auto& loaded_module : modules_)
    {
        const Module& module = loaded_module.get_module();
        const std::size_t text_address = reinterpret_cast<std::size_t>(module.get_text().data());
        if (address >= text_address && address < text_address + module.get_header().code_size())
        {
            return reinterpret_cast<std::size_t>(module.get_lot().get());
        }
    }

    // TODO: return somehow hard fault
    return 0xffffffff;
}

uint32_t DynamicLinker::get_size_of_lot(const ModuleHeader& header)
{
    return header.number_of_external_symbols() + header.number_of_local_relocations();
}

bool DynamicLinker::allocate_lot(LoadedModule& loaded_module)
{
    Module& module = loaded_module.get_module();
    const ModuleHeader& header = module.get_header();
    auto& lot = module.get_lot();
    lot.reset(new std::size_t[header.number_of_external_relocations() + header.number_of_local_relocations()]);
    return lot != nullptr;
}

void DynamicLinker::process_local_relocations(std::size_t local_relocations_address, LoadedModule& loaded_module)
{
    Module& module = loaded_module.get_module();
    const ModuleHeader& header = module.get_header();
    auto& lot = module.get_lot();

    for (int i = 0; i < header.number_of_local_relocations(); ++i)
    {
        const Relocation& relocation = *reinterpret_cast<const Relocation*>(local_relocations_address);
        local_relocations_address += relocation.size();
        uint32_t lot_index = relocation.index() >> 1;
        uint16_t section_value = relocation.index() & 0x01;
        Section section = static_cast<Section>(section_value);
        if (section == Section::code)
        {
            const std::size_t relocated = reinterpret_cast<std::size_t>(module.get_text().data()) + relocation.offset();
            lot[lot_index] = relocated;
        }
        else
        {
            const std::size_t relocated = reinterpret_cast<std::size_t>(module.get_data().data()) + relocation.offset();
            lot[lot_index] = relocated;
        }
    }
}

std::size_t DynamicLinker::get_relocations_size(const ModuleHeader& header)
{
    return sizeof(Relocation) * header.number_of_relocations();
}

std::size_t DynamicLinker::get_symbols_size(const std::size_t address, const uint32_t number_of_symbols)
{
    std::size_t size = 0;

    const Symbol* symbol = reinterpret_cast<const Symbol*>(address);
    for (uint32_t i = 0; i < number_of_symbols; ++i)
    {
        size += symbol->size();
        symbol = &symbol->next();
    }
    return size;
}

bool DynamicLinker::process_external_relocations(std::size_t external_relocations_address, const SymbolEntry* entries, std::size_t number_of_entries, LoadedModule& loaded_module)
{
    Module& module = loaded_module.get_module();
    const ModuleHeader& header = module.get_header();
    auto& lot = module.get_lot();

    bool status = true;
    for (int i = 0; i < header.number_of_external_relocations(); ++i)
    {
        const Relocation& relocation = *reinterpret_cast<const Relocation*>(external_relocations_address);

        external_relocations_address += relocation.size();
        const Symbol& symbol = relocation.symbol();
        const auto* env_symbol = find_symbol(entries, number_of_entries, symbol.code());
        if (env_symbol)
        {
            lot[relocation.index()] = env_symbol->address;
        }
        else
        {
            printf("Can't find symbol: %ld\n", symbol.code()); 
            status = false;
        }
    }
    return status;
}

void DynamicLinker::process_data_relocations(std::size_t data_relocations_address, LoadedModule& loaded_module)
{
    Module& module = loaded_module.get_module();
    const ModuleHeader& header = module.get_header();
    for (int i = 0; i < header.number_of_data_relocations(); ++i)
    {
        const Relocation& relocation = *reinterpret_cast<const Relocation*>(data_relocations_address);
        data_relocations_address += relocation.size();

        std::size_t *to_relocate = reinterpret_cast<std::size_t*>(reinterpret_cast<std::size_t>(module.get_data().data())) + relocation.index();

        std::size_t relocated = reinterpret_cast<std::size_t>(module.get_data().data()) + relocation.offset();
        *to_relocate = relocated;
    }
}


const SymbolEntry* DynamicLinker::find_symbol(const SymbolEntry* entries, std::size_t number_of_entries, uint32_t symbol_code)
{
    for (std::size_t i = 0; i < number_of_entries; ++i)
    {
        if (entries[i].code == symbol_code)
        {
            return &entries[i];
        }
    }
    return nullptr;
}


} // namespace dl
} // namespace msos
