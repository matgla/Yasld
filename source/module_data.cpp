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

#include "msos/dynamic_linker/module_data.hpp"

namespace msos
{
namespace dl
{

uint8_t* ModuleData::allocate_text(const std::size_t size)
{
    text_.reset(new uint8_t[size]);
    return text_.get();
}

uint8_t* ModuleData::allocate_data(const std::size_t size)
{
    data_.reset(new uint8_t[size]);
    return data_.get();
}

} // namespace dl
} // namespace msos

