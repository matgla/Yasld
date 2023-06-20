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

include(AddTestExecutable)

set(sources
    main.cpp)

add_test_executable(stm32f0_return_value_test ${sources})

set(renode_test_binary
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_return_value_test_with_host.bin)
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0_return_value_test.robot
  ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_return_value_test.robot @ONLY)
