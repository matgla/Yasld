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

namespace msos
{
namespace dl
{

class ModuleHeader
{
public:
    std::string_view cookie() const;
    uint32_t code_size() const;
    uint32_t data_size() const;
    uint32_t bss_size() const;
    constexpr uint16_t number_of_exported_relocations() const
    {
        return 0;
    }
    uint16_t number_of_external_relocations() const;
    uint16_t number_of_local_relocations() const;
    uint16_t number_of_data_relocations() const;
    uint32_t number_of_relocations() const;
    constexpr uint16_t number_of_exported_symbols() const
    {
        return 1;
    }
    uint16_t number_of_external_symbols() const;
    // std::string_view name() const;
    constexpr std::size_t size() const
    {
        return sizeof(ModuleHeader);
    }
private:
    char cookie_[4];
    uint32_t code_size_;
    uint32_t data_size_;
    uint32_t bss_size_;
    uint16_t number_of_external_relocations_;
    uint16_t number_of_local_relocations_;
    uint16_t number_of_data_relocations_;
    uint16_t number_of_external_symbols_;
};

} // namespace dl
} // namespace msos

