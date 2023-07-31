/**
 * align_tests.cpp
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

#include <gtest/gtest.h>

#include "yasld/align.hpp"

namespace yasld
{

TEST(AlignShould, CorrectlyAlignAddress)
{
  EXPECT_EQ(align(0, 3), 0);
  EXPECT_EQ(align(15, 16), 16);
  EXPECT_EQ(align(1, 4), 4);
  EXPECT_EQ(align(4, 4), 4);
}

TEST(AlignTemplateShould, CorrectlyAlignAddress)
{
  EXPECT_EQ((align<int, 3>(0)), 0);
  EXPECT_EQ((align<int, 16>(15)), 16);
  EXPECT_EQ((align<int, 4>(1)), 4);
  EXPECT_EQ((align<int, 4>(4)), 4);
}

} // namespace yasld
