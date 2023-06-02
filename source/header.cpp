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

#include "yasld/relocation.hpp"
#include "yasld/symbol.hpp"

namespace yasld
{

// TODO(matgla): add ifdef for printf usage

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
  printf("Cookie: %.4s\n", header.cookie);
  printf("Type: %s\n", toString(header.type).data());
  printf("Architecture: %s\n", toString(header.arch).data());
  printf("Yasiff version: %d\n", header.yasiff_version);
  printf("Sections size:\n");
  printf("  .text: %d B\n", header.code_length);
  printf("  .data: %d B\n", header.data_length);
  printf("  .bss:  %d B\n", header.bss_length);
  printf("External libraries amount: %d\n", header.external_libraries_amount);
  printf("Version: %d.%d\n", header.version_minor, header.version_minor);
  printf("External relocations amount: %d\n", header.external_relocations_amount);
  printf("Local relocations amount: %d\n", header.local_relocations_amount);
  printf("Data relocations amount: %d\n", header.data_relocations_amount);
  printf("Exported relocations amount: %d\n", header.exported_relocations_amount);
  printf("Exported symbols amount: %d\n", header.exported_symbols_amount);
  printf("External symbols amount: %d\n", header.external_symbols_amount);
  const uint8_t *ptr = reinterpret_cast<const uint8_t *>(&header) + sizeof(Header);

  ptr += header.external_relocations_amount * sizeof(Relocation);
  ptr += header.local_relocations_amount * sizeof(Relocation);
  ptr += header.data_relocations_amount * sizeof(Relocation);
  ptr += header.external_relocations_amount * sizeof(Relocation);
  ptr -= sizeof(Relocation);

  printf("Exported symbols:\n");
  const Symbol *symbol = reinterpret_cast<const Symbol *>(ptr);
  for (int i = 0; i < header.exported_symbols_amount; ++i)
  {
    printf("%s: %x\n", symbol->name().data(), symbol->offset());
    symbol = symbol->next();
  }
}

} // namespace yasld
