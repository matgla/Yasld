/**
 * parser_tests.cpp
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

#include "yasld/parser.hpp"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cstdint>
#include <ostream>
#include <vector>

#include "yasld/header.hpp"
#include "yasld/relocation.hpp"
#include "yasld/symbol.hpp"
//  0x04, 0x00,             // exported relocations amount
// 0x00, 0xbb, 0xcc, 0xdd, // exported relocation 1 index
// 0x01, 0x02, 0x03, 0x04, // exported relocation 1 offset
// 0x10, 0x2b, 0xc2, 0x2d, // exported relocation 2 index
// 0x11, 0x22, 0x23, 0x24, // exported relocation 2 offset
// 0x03, 0xb3, 0x3c, 0x3d, // exported relocation 3 index
// 0x04, 0x04, 0x04, 0x04, // exported relocation 3 offset
// 0x15, 0x25, 0xc5, 0x25, // exported relocation 4 index
// 0x15, 0x52, 0x53, 0x54, // exported relocation 4 offset

const std::vector<uint8_t> example_header = {
  0x59, 0x41, 0x46, 0x46, // YAFF
  0x01,                   // executable
  0x00, 0x01,             // armv6-m
  0x01,                   // YASIFF version 1
  0x04, 0x00, 0x00, 0x00, // code length = 4B
  0x02, 0x00, 0x00, 0x00, // data length = 2B
  0x08, 0x00, 0x00, 0x00, // bss length
  0x00, 0x00,             // external libraries
  0x00, 0x00,             // reserved
  0x0a, 0x00,             // version major
  0x05, 0x10,             // version minor
  0x03, 0x00,             // external relocations amount
  0x02, 0x00,             // local relocations amount
  0x01, 0x00,             // data relocations amount
  0x02, 0x00,             // exported symbols amount
  0x02, 0x00,             // external symbols amount
  0x00, 0x00, 0x00, 0x00, // external relocation 1 index
  0x45, 0x00, 0x00, 0x00, // external relocation 1 offset
  0x1e, 0x4b, 0xc1, 0x2d, // external relocation 2 index
  0x1e, 0x42, 0x21, 0x24, // external relocation 2 offset
  0x0e, 0x43, 0x31, 0x2d, // external relocation 3 index
  0x0e, 0x44, 0x01, 0x24, // external relocation 3 offset
  0x2e, 0x4b, 0xc1, 0x2d, // local relocation 1 index
  0x3e, 0x42, 0x21, 0x24, // local relocation 1 offset
  0x4e, 0x43, 0x31, 0x2d, // local relocation 2 index
  0x5e, 0x44, 0x01, 0x24, // local relocation 2 offset
  0x00, 0x00, 0x31, 0x2d, // data relocation 1 index
  0x5e, 0x12, 0x01, 0x24, // data relocation 1 offset
  0x12, 0x23, 0x34, 0x51, // exported symbol 1 - section data
  'a',  'b',  'c',  'd',
  '\0',                   // exported symbol 1 name
  0x00, 0x00, 0x11, 0x10, // exported symbol 2 - section code
  't',  'h',  'a',  't',  //
  'f',  'u',  'n',  '\0', // exported symbol 2 name
  0x02, 0x03, 0x04, 0x51, // external symbol 1 - section data
  'z',  't',  'a',  'd',
  '\0',                   // external symbol 1 name
  0x00, 0x01, 0x11, 0x10, // external symbol 2 - section code
  'h',  'e',  'y',  'y',  //
  'o',  'u',  '_',  '\0', // external symbol 2 name
};

namespace yasld
{

void PrintTo(const yasld::Relocation &rel, ::std::ostream *os)
{
  *os << "Relocation { "
      << ".index: 0x" << std::hex << rel.index() << ", "
      << ".offset: 0x" << std::hex << rel.symbol_offset() << " }";
}

void PrintTo(const std::span<const Relocation> &rels, ::std::ostream *os)
{
  *os << "[ ";
  for (const Relocation &rel : rels)
  {
    PrintTo(rel, os);
    *os << "," << std::endl;
  }
  *os << "]";
}

} // namespace yasld

TEST(ParserShould, ReportSymbolsAndRelocations)
{
  const yasld::Header *header(
    reinterpret_cast<const yasld::Header *>(example_header.data()));
  yasld::Parser parser(header);
  const auto    external_rels = parser.get_external_relocations();
  EXPECT_EQ(
    external_rels.address(),
    reinterpret_cast<std::uintptr_t>(example_header.data()) +
      sizeof(yasld::Header));
  EXPECT_EQ(external_rels.size(), 3 * sizeof(yasld::Relocation));

  const auto external_rels_span = external_rels.span();
  EXPECT_EQ(external_rels_span.size(), 3);

  EXPECT_EQ(external_rels_span[0], yasld::Relocation(0, 0x45));
  const auto external_symbol_0 = external_rels_span[0].symbol();

  std::cerr << "test: 0x" << std::hex
            << reinterpret_cast<std::size_t>(&external_rels_span[0])
            << std::endl
            << ", 0x" << reinterpret_cast<std::size_t>(&external_symbol_0)
            << std::endl;

  EXPECT_EQ(external_symbol_0.name(), "ztad");

  //   EXPECT_THAT(
  //     external_rels_span,
  //     ::testing::ElementsAreArray({
  //       yasld::Relocation{ 0x2dc14b0e, 0x2401420e},
  //       yasld::Relocation{ 0x2dc14b1e, 0x},
  //       yasld::Relocation{ 0xe4bc12d, 0xe420124},
  //   }));
}
