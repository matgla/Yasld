/**
 * header.cpp
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

#include "yasld/header.hpp"

#include <cstdio>
#include <string_view>

#include "yasld/logger.hpp"
#include "yasld/relocation.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

// TODO(matgla): add ifdef for log usage

std::string_view toString(const Header::Type type)
{
  switch (type)
  {
  case Header::Type::Library:
    return "Library";
  case Header::Type::Executable:
    return "Executable";
  default:
    return "Unknown";
  }
}

std::string_view toString(const Header::Architecture architecture)
{
  switch (architecture)
  {
  case Header::Architecture::ArmV6_M:
    return "armv6-m";
  default:
    return "unknown";
  }
}

void print(const Header &header)
{
  log("Cookie: %.4s\n", header.cookie);
  log("Type: %s\n", toString(header.type).data());
  log("Architecture: %s\n", toString(header.arch).data());
  log("Yasiff version: %d\n", header.yasiff_version);
  log("Sections size:\n");
  log("  .text: %d B\n", header.code_length);
  log("  .data: %d B\n", header.data_length);
  log("  .bss:  %d B\n", header.bss_length);
  log("External libraries amount: %d\n", header.external_libraries_amount);
  log("Version: %d.%d\n", header.version_minor, header.version_minor);
  log("External relocations amount: %d\n", header.external_relocations_amount);
  log("Local relocations amount: %d\n", header.local_relocations_amount);
  log("Data relocations amount: %d\n", header.data_relocations_amount);
  // log("Exported relocations amount: %d\n",
  // header.exported_relocations_amount);
  log("Exported symbols amount: %d\n", header.exported_symbols_amount);
  log("External symbols amount: %d\n", header.external_symbols_amount);
  const uint8_t *ptr =
    reinterpret_cast<const uint8_t *>(&header) + sizeof(Header);

  ptr += header.external_relocations_amount * sizeof(Relocation);
  ptr += header.local_relocations_amount * sizeof(Relocation);
  ptr += header.data_relocations_amount * sizeof(Relocation);
  ptr += header.external_relocations_amount * sizeof(Relocation);

  log("Exported symbols: %d\n", header.exported_symbols_amount);
  const Symbol *symbol = reinterpret_cast<const Symbol *>(ptr);
  for (int i = 0; i < header.exported_symbols_amount; ++i)
  {
    log("%s: %x\n", symbol->name().data(), symbol->offset());
    symbol = symbol->next(header.alignment);
  }
}

} // namespace yasld
