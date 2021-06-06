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

enum class SymbolVisibility : uint16_t
{
    internal = 0,
    exported = 1,
    external = 2
};

enum class Section : uint16_t
{
    code = 0,
    data = 1
};

std::string_view to_string(const SymbolVisibility v);
std::string_view to_string(const Section s);

class Symbol
{
public:
    Section section() const;
    uint32_t code() const;
    const Symbol& next() const;
    uint32_t offset() const;
    constexpr std::size_t size() const
    {
        return sizeof(Symbol);
    }
private:
    uint16_t size_;
    Section section_;
    uint32_t offset_;
    uint32_t code_;
};

} // namespace dl
} // namespace msos

