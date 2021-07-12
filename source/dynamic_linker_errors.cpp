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

#include "msos/dynamic_linker/dynamic_linker_errors.hpp"

#include <eul/error/error_category.hpp>

namespace msos
{
namespace dl
{

class DynamicLinkerErrorCategory : public eul::error::error_category
{
public:
    std::string_view name() const noexcept override
    {
        return "DynamicLinker";
    }

    std::string_view message(int id) const noexcept override
    {
        switch (static_cast<DynamicLinkerErrors>(id))
        {
            case DynamicLinkerErrors::CookieValidationFailure: return "Wrong cookie";
            case DynamicLinkerErrors::TextAllocationFailure: return "Out of memory";
            case DynamicLinkerErrors::DataAllocationFailure: return "Out of memory";
            case DynamicLinkerErrors::LotAllocationFailure: return "Out of memory";
            case DynamicLinkerErrors::ExternalRelocationFailure: return "External relocation";
            default: return "Unknown";
        }
    }
};

static DynamicLinkerErrorCategory category_;

eul::error::error_code make_error_code(DynamicLinkerErrors e)
{
    return eul::error::error_code(static_cast<int>(e), category_);
}

} // namespace dl
} // namespace msos
