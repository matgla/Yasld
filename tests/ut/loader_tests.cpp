/**
 * loader_tests.cpp
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

#include <array>
#include <gtest/gtest.h>

#include "yasld/loader.hpp"

namespace yasld
{

alignas(16) const std::array<uint8_t, 128 * 4> example_header = {
  0x59, 0x41, 0x46, 0x46, // YAFF
  0x01, 0x00, 0x01, 0x01, // executable, armv6-m, YASIFF version 1
  0x20, 0x00, 0x00, 0x00, // code length = 32B
  0x18, 0x00, 0x00, 0x00, // data length = 16B

  0x08, 0x00, 0x00, 0x00, // bss length
  0x00, 0x00, 0x08, 0x00, // external libraries, alignment: 8, reserved
  0x0a, 0x00, 0x05, 0x10, // version major, minor
  0x00, 0x00, 0x02, 0x00, // external, local relocations amount

  0x02, 0x00, 0x01, 0x00, // data relocations amount, exported symbols amount
  0x01, 0x00, 0x00, 0x00, // external symbols amount, alignment to 8

  0x01, 0x00, 0x00, 0x00, // local relocation 1 index from data
  0x04, 0x00, 0x00, 0x00, // local relocation 1 offset
  0x02, 0x00, 0x00, 0x00, // local relocation 2 index from code
  0x08, 0x00, 0x00, 0x00, // local relocation 2 offset

  0x08, 0x00, 0x00, 0x00, // data relocation 1 index from data
  0x20, 0x00, 0x00, 0x00, // data relocation 1 offset based on code
  0x00, 0x00, 0x00, 0x00, // data relocation 2 index from data
  0x41, 0x00, 0x00, 0x00, // data relocation 2 offset based on data
  0x07, 0x00, 0x00, 0x00, // exported symbol 1 - section data
  'm',  'a',  'i',  'n',  // exported symbol 1 - name

  '\0', 0x00, 0x00, 0x00, // exported symbol 1 name
  0x00, 0x00, 0x00, 0x00, // alignment to 8
  0x02, 0x00, 0x00, 0x00, // external symbol 1 - section code
  'z',  't',  'a',  '\0', // external symbol 1 name

  0x01, 0x02, 0x03, 0x04, // text
  0x05, 0x06, 0x07, 0x08, // text
  0x09, 0x0a, 0x0b, 0x0c, // text
  0x0d, 0x0e, 0x0f, 0x10, // text
  0x21, 0x22, 0x23, 0x24, // text
  0x25, 0x26, 0x27, 0x28, // text
  0x29, 0x2a, 0x2b, 0x2c, // text
  0x2d, 0x2e, 0x2f, 0x20, // text

  0x0a, 0x00, 0x00, 0x00, // data
  0x0b, 0x00, 0x00, 0x00, // data
  0x0c, 0x00, 0x00, 0x00, // data
  0x0d, 0x00, 0x00, 0x00, // data
  0x01, 0x02, 0x03, 0x04, // data
  0x05, 0x06, 0x07, 0x08, // data
};

std::array<std::size_t, 3> lot;
std::array<uint8_t, 16>    data_and_bss;
TEST(LoaderShould, LoadExecutable)
{
  Loader loader(
    [](std::size_t size)
    {
      static int counter = 0;
      if (counter == 0)
      {
        counter++;
        return std::span<std::byte>(
          reinterpret_cast<std::byte *>(lot.data()), lot.size());
      }
      return std::span<std::byte>(
        reinterpret_cast<std::byte *>(data_and_bss.data()),
        data_and_bss.size());
    },
    [](std::span<std::byte> data)
    {
      free(data.data());
    });

  const auto exec =
    loader.load_executable(example_header.data(), Loader::Mode::copy_only_data);

  EXPECT_EQ(lot[0], reinterpret_cast<std::size_t>(data_and_bss.data()) + 4);

  const std::size_t offset_to_text = 24 * 4;
  EXPECT_EQ(
    lot[1],
    reinterpret_cast<std::size_t>(example_header.data()) + offset_to_text + 8);

  // Verify that data relocation towards code was processed correctly
  EXPECT_EQ(
    *reinterpret_cast<std::size_t *>(
      reinterpret_cast<std::size_t>(data_and_bss.data()) + 8),
    reinterpret_cast<std::size_t>(example_header.data()) + offset_to_text + 16);

  // Verify that data relocation towards data was processed correctly
  EXPECT_EQ(
    *reinterpret_cast<std::size_t *>(
      reinterpret_cast<std::size_t>(data_and_bss.data())),
    reinterpret_cast<std::size_t>(data_and_bss.data()) + 32);

  EXPECT_EQ(
    *reinterpret_cast<std::size_t *>(
      reinterpret_cast<std::size_t>(data_and_bss.data()) + 16),
    0x807060504030201);

  // Executable verification
  EXPECT_TRUE(exec);
  EXPECT_EQ(
    exec->text_address(),
    reinterpret_cast<std::size_t>(example_header.data()) + offset_to_text);

  EXPECT_EQ(
    *loader.find_symbol("main"),
    reinterpret_cast<std::size_t>(data_and_bss.data()) + 3);

  EXPECT_FALSE(loader.find_symbol("not_existing"));
}

} // namespace yasld
