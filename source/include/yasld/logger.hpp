/**
 * logger.hpp
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

#ifndef LOGGER_ENABLED
#define LOGGER_ENABLED 0
#endif // LOGGER_ENABLED

#if (LOGGER_ENABLED == 1)
#include <cstdio>
#define log(...) printf("[YASLD] " __VA_ARGS__)
namespace yasld
{
    constexpr bool logger_enabled = true;
} // namespace yasld
#else
#define log(...)
namespace yasld
{
    constexpr bool logger_enabled = false;
} // namespace yasld
#endif
