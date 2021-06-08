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
#include <array>
#include <string_view>

extern "C"
{

typedef struct SymbolEntry
{
    uint32_t code;
    std::size_t address;
} SymbolEntry;

}

namespace msos
{
namespace dl
{

class SymbolAddress
{
public:
    template <typename R, typename... Args>
    SymbolAddress(uint32_t code, const R*(*function)(Args...)) noexcept
        : entry_{code, reinterpret_cast<std::size_t>(function)}
    {

    }

    template <typename R, typename... Args>
    SymbolAddress(uint32_t code, R*(*function)(Args...)) noexcept
        : entry_{code, reinterpret_cast<std::size_t>(function)}
    {

    }

    template <typename T>
    SymbolAddress(uint32_t code, const T* data) noexcept
        : entry_{code, reinterpret_cast<std::size_t>(data)}
    {

    }

    template <typename T>
    SymbolAddress(uint32_t code, T* data) noexcept
        : entry_{code, reinterpret_cast<std::size_t>(data)}
    {

    }

    uint32_t code() const
    {
        return entry_.code;
    }

    std::size_t address() const
    {
        return entry_.address;
    }

private:
    SymbolEntry entry_;
};

template <std::size_t N>
class Environment
{
public:
    template <typename... Args>
    Environment(Args&&... args) : data_({std::forward<Args>(args)...})
    {
    }

    const SymbolAddress* find_symbol(uint32_t symbol_code) const
    {
        for (const auto& symbol : data_)
        {
            if (symbol.code() == symbol_code)
            {
                return &symbol;
            }
        }
        return nullptr;
    }

    std::array<SymbolAddress, N>& data()
    {
        return data_;
    }

    const std::array<SymbolAddress, N>& data() const
    {
        return data_;
    }
private:
    std::array<SymbolAddress, N> data_;
};

template <typename... Args>
Environment(Args&&...) -> Environment<sizeof...(Args)>;

} // namespace dl
} // namespace msos
