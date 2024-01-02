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

include(AddTestWithModule)

add_test_with_module(
  NAME
  stm32f0_nucleo_f091_rc_print_hello_world
  MODULE
  stm32f0_hello_world
  SOURCES
  main.cpp
  SCRIPTS
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  LIBRARIES
  test::st::cortex_m0_plus::nucleo_f091rc
  ROBOT_COMMON_FILE
  stm32f0_nucleo_f091rc_common.robot
  BOARD_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../nucleo_f091rc.repl)
