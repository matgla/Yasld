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

#include <memory>

#include "msos/dynamic_linker/module.hpp"

namespace msos
{
namespace dl
{

class LoadedModule
{
public:
    LoadedModule();
    LoadedModule(const ModuleHeader& header);

    const Module& get_module() const;
    Module& get_module();

    void set_start_address(const std::size_t start_address);
    int execute(int argc, char *argv[]) const;
    int execute(int argc, const char *argv[]) const;
    int execute() const;

// private:
    std::size_t start_address_;
    Module module_;
};

} // namespace dl
} // namespace msos

