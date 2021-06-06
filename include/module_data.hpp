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
#include <memory>

#include <gsl/span>

namespace msos
{
namespace dl
{

class ModuleData
{
public:
    using DataSpan = gsl::span<uint8_t>;

    uint8_t* allocate_text(const std::size_t size);
    uint8_t* allocate_data(const std::size_t size);

public:
    std::unique_ptr<uint8_t[]> text_;
    std::unique_ptr<uint8_t[]> data_;
};

} // namespace dl
} // namespace msos

