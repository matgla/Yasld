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

#include "msos/dynamic_linker/loaded_module.hpp"

#include <utility>

#include "msos/dynamic_linker/module_data.hpp"


extern "C"
{
int call_external(std::size_t address, void* lot);
int call_external_with_args(int argc, char *argv[], std::size_t address, void* lot);
}

namespace msos
{
namespace dl
{

LoadedModule::LoadedModule()
{
}

LoadedModule::LoadedModule(const ModuleHeader& header)
    : module_(header)
{
}

const Module& LoadedModule::get_module() const
{
    return module_;
}

Module& LoadedModule::get_module()
{
    return module_;
}

void LoadedModule::set_start_address(const std::size_t start_address)
{
    start_address_ = start_address;
}

int LoadedModule::execute(int argc, char *argv[]) const
{
    return call_external_with_args(argc, argv, start_address_, module_.get_lot().get());
}

int LoadedModule::execute(int argc, const char *argv[]) const
{
    return call_external_with_args(argc, const_cast<char**>(argv), start_address_, module_.get_lot().get());
}


int LoadedModule::execute() const
{
    return call_external(start_address_, module_.get_lot().get());
}

} // namespace dl
} // namespace msos

