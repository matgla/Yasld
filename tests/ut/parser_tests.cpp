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
  0x08,                   // alignment
  0x00,                   // reserved
  0x0a, 0x00,             // version major
  0x05, 0x10,             // version minor
  0x03, 0x00,             // external relocations amount
  0x02, 0x00,             // local relocations amount
  0x01, 0x00,             // data relocations amount
  0x02, 0x00,             // exported symbols amount
  0x02, 0x00,             // external symbols amount
  0x00, 0x00,             // alignment to 8
  0x00, 0x00, 0x00, 0x00, // external symbol relocation 1 index
  0x50, 0x00, 0x00, 0x00, // external symbol relocation 1 offset
  0x01, 0x00, 0x00, 0x00, // external symbol relocation 2 index
  0x50, 0x00, 0x00, 0x00, // external symbol relocation 2 offset
  0x02, 0x00, 0x00, 0x00, // external symbol relocation 3 index
  0x58, 0x00, 0x00, 0x00, // external symbol relocation 3 offset
  0x07, 0x00, 0x00, 0x00, // local relocation 1 index
  0x30, 0x42, 0x21, 0x24, // local relocation 1 offset
  0x08, 0x00, 0x00, 0x00, // local relocation 2 index
  0x5e, 0x44, 0x01, 0x24, // local relocation 2 offset
  0x05, 0x00, 0x00, 0x00, // data relocation 1 index
  0x5e, 0x12, 0x01, 0x24, // data relocation 1 offset
  0x12, 0x23, 0x34, 0x51, // exported symbol 1 - section data
  'a',  'b',  'c',  'd',  // exported symbol 1 - name
  '\0', 0x00, 0x00, 0x00, // exported symbol 1 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x00, 0x00, 0x11, 0x10, // exported symbol 2 - section code
  't',  'h',  'a',  't',  //
  'f',  'u',  'n',  '\0', // exported symbol 2 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x02, 0x03, 0x04, 0x51, // external symbol 1 - section data
  'z',  't',  'a',  '\0', // external symbol 1 name
  0x00, 0x01, 0x11, 0x10, // external symbol 2 - section code
  'h',  'e',  'y',  'y',  //
  'o',  'u',  '_',  '\0', // external symbol 2 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x02, 0x03, 0x03, 0x42, // external symbol 3 - section code
  'n',  'a',  'm',  'e',  //
  'i',  's',  't',  'h',  //
  'e',  'f',  'u',  'n',  //
  'c',  't',  'i',  'o',  //
  'n',  0x00, 0x00, 0x00
};

namespace yasld
{

void PrintTo(const yasld::Relocation &rel, ::std::ostream *os)
{
  *os << "Relocation { "
      << ".index: 0x" << std::hex << rel.lot_index() << ", "
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

template <typename T>
T align(const T address, std::size_t alignment)
{
  if (address % alignment != 0)
  {
    return address + (alignment - address % alignment);
  }
  return address;
}

TEST(ParserShould, ParseExternalRelocationTable)
{
  const yasld::Header *header(
    reinterpret_cast<const yasld::Header *>(example_header.data()));
  yasld::Parser parser(header);
  const auto    external_rels = parser.get_external_relocations();
  // Alignment to 8 enforced in header to not crash on 64 bit machines
  ASSERT_EQ(header->alignment, 8);

  EXPECT_EQ(
    external_rels.address(),
    reinterpret_cast<std::uintptr_t>(example_header.data()) +
      align(sizeof(yasld::Header), header->alignment));

  EXPECT_EQ(external_rels.size(), 3 * sizeof(yasld::Relocation));

  const auto external_rels_span = external_rels.span();
  EXPECT_EQ(external_rels_span.size(), 3);

  const auto &r0 = external_rels_span[0];
  EXPECT_EQ(r0, yasld::Relocation(0, 0x50));
  const auto &es0 = external_rels_span[0].symbol();
  EXPECT_EQ(es0.name(), "zta");

  const auto &r1 = external_rels_span[1];
  const auto &s1 = r1.symbol();

  EXPECT_EQ(r1, yasld::Relocation(1, 0x50));
  EXPECT_EQ(s1.name(), "heyyou_");

  const auto &r2 = external_rels_span[2];
  const auto &s2 = r2.symbol();

  EXPECT_EQ(r2, yasld::Relocation(2, 0x58));
  EXPECT_EQ(s2.name(), "nameisthefunction");
}

TEST(ParserShould, ParseLocalRelocationTable)
{
  const yasld::Header *header(
    reinterpret_cast<const yasld::Header *>(example_header.data()));
  yasld::Parser parser(header);
  const auto    local_rels = parser.get_local_relocations();
  // Alignment to 8 enforced in header to not crash on 64 bit machines
  ASSERT_EQ(header->alignment, 8);

  EXPECT_EQ(
    local_rels.address(),
    reinterpret_cast<std::uintptr_t>(example_header.data()) +
      align(
        header->external_relocations_amount * sizeof(yasld::Relocation), 8) +
      align(sizeof(yasld::Header), header->alignment));

  EXPECT_EQ(local_rels.size(), 2 * sizeof(yasld::LocalRelocation));
  const auto local_rels_span = local_rels.span();

  EXPECT_EQ(local_rels_span.size(), 2);

  const auto &r0 = local_rels_span[0];
  EXPECT_EQ(r0.lot_index(), 3);
  EXPECT_EQ(r0.offset(), 0x24214230);
  EXPECT_EQ(r0.section(), yasld::Section::data);

  const auto &r1 = local_rels_span[1];
  EXPECT_EQ(r1.lot_index(), 4);
  EXPECT_EQ(r1.offset(), 0x2401445e);
  EXPECT_EQ(r1.section(), yasld::Section::code);
}
