// This file is part of msos project.
// Copyright (C) 2021 Mateusz Stadnik
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

#include "msos/dynamic_linker/loaded_module.hpp"
#include "msos/dynamic_linker/symbol.hpp"

namespace msos::dl 
{

DynamicLinker::DynamicLinker()
{

}

const Symbol* DynamicLinker::find_symbol(const std::size_t address, const uint32_t number_of_symbols, uint32_t symbol_code)
{
    return nullptr;
}

const LoadedModule* DynamicLinker::load_module(const std::size_t *module_address, const int mode, const SymbolEntry *entries, std::size_t number_of_entries, eul::error::error_code &ec)
{
    return nullptr; 
}

} // namespace msos::dl
