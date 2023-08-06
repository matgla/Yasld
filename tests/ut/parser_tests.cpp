/**
 * parser_tests.cpp
 *
 * Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
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

alignas(16) const std::vector<uint8_t> example_header = {
  0x59, 0x41, 0x46, 0x46, // YAFF
  0x01, 0x00, 0x01, 0x01, // executable, armv6-m, YASIFF version 1
  0x04, 0x00, 0x00, 0x00, // code length = 4B
  0x02, 0x00, 0x00, 0x00, // data length = 2B
  0x08, 0x00, 0x00, 0x00, // bss length
  0x00, 0x00, 0x08, 0x00, // external libraries, alignment: 8, reserved
  0x0a, 0x00, 0x05, 0x10, // version major, minor
  0x03, 0x00, 0x02, 0x00, // external, local relocations amount
  0x01, 0x00, 0x02, 0x00, // data relocations amount, exported symbols amount
  0x03, 0x00, 0x00, 0x00, // external symbols amount, alignment to 8
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
  0x13, 0x23, 0x34, 0x51, // exported symbol 1 - section data
  'a',  'b',  'c',  'd',  // exported symbol 1 - name
  '\0', 0x00, 0x00, 0x00, // exported symbol 1 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x00, 0x00, 0x11, 0x10, // exported symbol 2 - section code
  't',  'h',  'a',  't',  //
  'f',  'u',  'n',  '\0', // exported symbol 2 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x02, 0x03, 0x04, 0x51, // external symbol 1 - section code
  'z',  't',  'a',  '\0', // external symbol 1 name
  0x00, 0x01, 0x11, 0x10, // external symbol 2 - section code
  'h',  'e',  'y',  'y',  //
  'o',  'u',  '_',  '\0', // external symbol 2 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x01, 0x03, 0x03, 0x42, // external symbol 3 - section data
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

class ParserShould : public ::testing::Test
{
public:
  ParserShould()
    : header_{ reinterpret_cast<const yasld::Header *>(example_header.data()) }
    , sut_{ header_ }
  {
  }

protected:
  const yasld::Header *header_;
  const yasld::Parser  sut_;
};

TEST_F(ParserShould, ParseExternalRelocationTable)
{
  const auto external_rels = sut_.get_external_relocations();
  // Alignment to 8 enforced in header to not crash on 64 bit machines
  ASSERT_EQ(header_->alignment, 8);

  EXPECT_EQ(
    external_rels.address(),
    align(
      reinterpret_cast<std::uintptr_t>(example_header.data()) +
        sizeof(yasld::Header),
      header_->alignment));

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

TEST_F(ParserShould, ParseLocalRelocationTable)
{
  const auto local_rels = sut_.get_local_relocations();
  // Alignment to 8 enforced in header to not crash on 64 bit machines
  ASSERT_EQ(header_->alignment, 8);

  EXPECT_EQ(
    local_rels.address(),
    reinterpret_cast<std::uintptr_t>(example_header.data()) +
      align(
        header_->external_relocations_amount * sizeof(yasld::Relocation), 8) +
      align(sizeof(yasld::Header), header_->alignment));

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

TEST_F(ParserShould, ParseExportedSymbolTable)
{
  const auto exported_symbols = sut_.get_exported_symbols();

  EXPECT_EQ(exported_symbols.size(), 32);
  auto it = exported_symbols.begin();
  EXPECT_EQ(it->name(), "abcd");
  EXPECT_EQ(it->section(), yasld::Section::data);
  EXPECT_EQ(it->offset(), 0x289a1189);
  ++it;
  EXPECT_EQ(it->name(), "thatfun");
  EXPECT_EQ(it->section(), yasld::Section::code);
  EXPECT_EQ(it->offset(), 0x8088000);
}

TEST_F(ParserShould, ParseExternalSymbolTable)
{
  const auto external_symbols = sut_.get_external_symbols();

  EXPECT_EQ(external_symbols.size(), 48);
  auto it = external_symbols.begin();
  EXPECT_EQ(it->name(), "zta");
  EXPECT_EQ(it->section(), yasld::Section::code);
  EXPECT_EQ(it->offset(), 0x28820181);
  ++it;
  EXPECT_EQ(it->name(), "heyyou_");
  EXPECT_EQ(it->section(), yasld::Section::code);
  EXPECT_EQ(it->offset(), 0x8088080);
  ++it;
  EXPECT_EQ(it->name(), "nameisthefunction");
  EXPECT_EQ(it->section(), yasld::Section::data);
  EXPECT_EQ(it->offset(), 0x21018180);
}

TEST_F(ParserShould, ParseTextSection)
{
  const auto text = sut_.get_text();

  EXPECT_EQ(text.size(), header_->code_length);

  const std::size_t offset =
    sizeof(yasld::Header) + sut_.get_external_relocations().size() +
    sut_.get_local_relocations().size() + sut_.get_data_relocations().size() +
    sut_.get_exported_symbols().size() + sut_.get_external_symbols().size();

  const std::size_t aligned_offset = offset + 16 - offset % 16;

  EXPECT_EQ(
    text.data(), reinterpret_cast<const std::byte *>(header_) + aligned_offset);
}

TEST_F(ParserShould, ParseDataSection)
{
  const auto data = sut_.get_data();

  EXPECT_EQ(data.size(), header_->data_length);

  const std::size_t text_offset =
    sizeof(yasld::Header) + sut_.get_external_relocations().size() +
    sut_.get_local_relocations().size() + sut_.get_data_relocations().size() +
    sut_.get_exported_symbols().size() + sut_.get_external_symbols().size();

  const std::size_t aligned_offset = text_offset + 16 - text_offset % 16;
  const std::size_t data_offset    = aligned_offset + header_->code_length;
  EXPECT_EQ(
    data.data(), reinterpret_cast<const std::byte *>(header_) + data_offset);
}
