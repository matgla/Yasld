/**
 * interface.hpp
 *
 * Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation, either version
 * 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General
 * Public License along with this program. If not, see
 * <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <string_view>

int __attribute__((visibility("default"))) sum(int a, int b);

class Foo
{
public:
  int __attribute__((visibility("default"))) bar(int a, int b, int c, int d);
};

extern "C"
{
  int __attribute__((visibility("default")))
  c_fun(int a, int b, int c, int d, int e);
}

namespace a::b::c
{
std::string_view __attribute__((visibility("default")))
process_str(const std::string_view &a, const std::string_view b);
}
