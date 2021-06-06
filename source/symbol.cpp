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

#include "msos/dynamic_linker/symbol.hpp"

namespace msos
{
namespace dl
{

std::string_view to_string(const SymbolVisibility v)
{
    switch (v)
    {
         case SymbolVisibility::internal: return "internal";
         case SymbolVisibility::exported: return "exported";
         case SymbolVisibility::external: return "external";
    }
    return "Unknown";
}

std::string_view to_string(const Section s)
{
    switch (s)
    {
        case Section::code: return "code";
        case Section::data: return "data";
    }
    return "Unknown";
}

Section Symbol::section() const
{
    return section_;
}

uint32_t Symbol::code() const
{
    return code_;
}

const Symbol& Symbol::next() const
{
    return *(this + 1);
}

uint32_t Symbol::offset() const
{
    return offset_;
}


} // namespace dl
} // namespace msos

