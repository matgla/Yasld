/**
 * header.hpp
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

#include <cstdint>

namespace yasld
{

struct __attribute__((packed)) Header
{
public:
  enum class Type : uint8_t
  {
    Unknown = 0,
    Executable = 1,
    Library = 2,
  };

  enum class Architecture : uint16_t
  {
    Unknown = 0,
    ArmV6_M = 1,
  };

  const char cookie[4];
  Type type;
  Architecture arch;
  uint8_t yasiff_version;
  uint32_t code_length;
  uint32_t data_length;
  uint32_t bss_length;
  uint16_t external_libraries_amount;
  uint16_t _reserved;
  uint16_t version_major;
  uint16_t version_minor;
  uint16_t external_relocations_amount;
  uint16_t local_relocations_amount;
  uint16_t data_relocations_amount;
  uint16_t exported_symbols_amount;
};

void print(const Header &header);

} // namespace yasld
