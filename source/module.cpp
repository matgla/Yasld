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

#include "msos/dynamic_linker/module.hpp"

#include <gsl/span>

#include "msos/dynamic_linker/module_header.hpp"
#include "msos/dynamic_linker/module_data.hpp"

namespace msos
{
namespace dl
{

Module::Module()
    : module_header_(nullptr)
{
}

Module::Module(const ModuleHeader& header)
    : module_header_(&header)
{
}

Module::DataSpan Module::get_text() const
{
    return gsl::span(text_, module_header_->code_size());
}


Module::DataSpan Module::get_data() const
{
    return gsl::span(data_, module_header_->data_size());
}

const ModuleHeader& Module::get_header() const
{
    return *module_header_;
}

const std::unique_ptr<size_t[]>& Module::get_lot() const
{
    return lot_;
}

std::unique_ptr<std::size_t[]>& Module::get_lot()
{
    return lot_;
}

ModuleData& Module::get_module_data()
{
    return module_data_;
}

const ModuleData& Module::get_module_data() const
{
    return module_data_;
}

void Module::set_text(const Module::DataSpan& text)
{
    text_ = text.data();
}

void Module::set_data(const Module::DataSpan& data)
{
    data_ = data.data();
}

bool Module::allocate_text()
{
    text_ = module_data_.allocate_text(module_header_->code_size());
    return text_ != nullptr;
}

bool Module::allocate_data()
{
    data_ = module_data_.allocate_data(module_header_->data_size() + module_header_->bss_size());
    return data_ != nullptr;
}

} // namespace dl
} // namespace msos

