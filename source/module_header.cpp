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

#include "msos/dynamic_linker/module_header.hpp"

#include "msos/usart_printer.hpp"

namespace msos
{
namespace dl
{

inline std::size_t get_aligned_length(const std::string_view& str)
{
    std::size_t name_length = str.length() + 1;
    if (name_length % 4)
    {
        name_length = name_length + 4 - (name_length % 4);
    }
    return name_length;
}


std::string_view ModuleHeader::cookie() const
{
    return std::string_view(cookie_, 4);
}

uint32_t ModuleHeader::code_size() const
{
    return code_size_;
}

uint32_t ModuleHeader::data_size() const
{
    return data_size_;
}

uint32_t ModuleHeader::bss_size() const
{
    return bss_size_;
}

uint16_t ModuleHeader::number_of_external_relocations() const
{
    return number_of_external_relocations_;
}

uint16_t ModuleHeader::number_of_local_relocations() const
{
    return number_of_local_relocations_;
}

uint16_t ModuleHeader::number_of_data_relocations() const
{
    return number_of_data_relocations_;
}

uint32_t ModuleHeader::number_of_relocations() const
{
    return number_of_external_relocations_ +
        number_of_local_relocations_ + number_of_data_relocations_;
}

uint16_t ModuleHeader::number_of_external_symbols() const
{
    return number_of_external_symbols_;
}

} // namespace dl
} // namespace msos

