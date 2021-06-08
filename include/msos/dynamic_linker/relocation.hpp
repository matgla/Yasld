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

#include "msos/dynamic_linker/fwd.hpp"

namespace msos
{
namespace dl
{

class Relocation
{
public:
    uint32_t index() const;
    const Symbol& symbol() const;
    constexpr static std::size_t size()
    {
        return sizeof(uint32_t) * 2;
    }
    const Relocation& next() const;
    uint32_t offset() const;
private:
    uint32_t index_;
    uint32_t symbol_offset_;
};

} // namespace dl
} // namespace msos

