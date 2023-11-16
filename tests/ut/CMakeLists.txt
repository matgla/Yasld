#
# CMakeLists.txt
#
# Copyright (C) 2023 Mateusz Stadnik <matgla@live.com>
#
# This program is free software: you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along with
# this program. If not, see <https://www.gnu.org/licenses/>.
#

find_package(googletest REQUIRED)

add_executable(yasld_ut)
target_sources(yasld_ut PRIVATE putchar.cpp align_tests.cpp parser_tests.cpp)
target_link_libraries(yasld_ut PUBLIC GTest::gtest_main GTest::gmock yasld)

add_test(NAME YasldUnitTests COMMAND yasld_ut)
