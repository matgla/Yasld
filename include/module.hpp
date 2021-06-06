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

#include <cstdint>
#include <string_view>
#include <vector>

#include <gsl/span>

#include "msos/dynamic_linker/fwd.hpp"
#include "msos/dynamic_linker/module_data.hpp"

namespace msos
{
namespace dl
{

class Module
{
public:
    Module();
    using DataSpan = gsl::span<uint8_t>;
    Module(const ModuleHeader& module_header);

    DataSpan get_text() const;
    DataSpan get_data() const;
    const ModuleHeader& get_header() const;
    const std::unique_ptr<size_t[]>& get_lot() const;
    std::unique_ptr<size_t[]>& get_lot();
    ModuleData& get_module_data();
    const ModuleData& get_module_data() const;

    void set_text(const DataSpan& text);
    void set_data(const DataSpan& data);

    bool allocate_text();
    bool allocate_data();

private:
    const ModuleHeader* module_header_;
    uint8_t* text_;
    uint8_t* data_;
    std::unique_ptr<size_t[]> lot_;
    ModuleData module_data_;
};

} // namespace dl
} // namespace msos

