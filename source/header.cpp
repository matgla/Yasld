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

#include <string_view>

#include "yasld/logger.hpp"

namespace yasld
{

std::string_view to_string(const Header::Type type)
{
  switch (type)
  {
  case Header::Type::Library:
    return "library";
  case Header::Type::Executable:
    return "executable";
  case Header::Type::Unknown:
    return "unknown";
  }
  return "unknown";
}

std::string_view to_string(const Header::Architecture arch)
{
  switch (arch)
  {
  case Header::Architecture::Armv6_m:
    return "armv6-m";
  case Header::Architecture::Unknown:
    return "unknown";
  }
  return "unknown";
}

void print(const Header &header)
{
  log("Cookie: %.4s\n", header.cookie);
  log("Type: %s\n", to_string(header.type).data());
  log("Architecture: %s\n", to_string(header.arch).data());
  log("Yasiff version: %u\n", header.yasiff_version);
  log("Sections size:\n");
  log("  .text: %u B\n", header.code_length);
  log("  .init: %u B\n", header.init_length);
  log("  .data: %u B\n", header.data_length);
  log("  .bss:  %u B\n", header.bss_length);
  log("External libraries amount: %u\n", header.external_libraries_amount);
  log("Alignment: %u\n", header.alignment);
  log("Version: %u.%u\n", header.version_major, header.version_minor);
  log("Relocations amount:\n");
  log("  symtab: %u\n", header.symbol_table_relocations_amount);
  log("  local:  %u\n", header.local_relocations_amount);
  log("  data:   %u\n", header.data_relocations_amount);
  log("Symbol table size:\n");
  log("  exported: %u\n", header.exported_symbols_amount);
  log("  external: %u\n", header.imported_symbols_amount);
}

} // namespace yasld
