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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(ConfigureWithToolchain)

if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/renode_st_tests.yaml)
  file(TOUCH ${CMAKE_CURRENT_BINARY_DIR}/renode_st_tests.yaml)
endif()

configure_with_toolchain(
  NAME
  stm32f0_disco
  ARGS
  -DST_TESTS_FILE=${CMAKE_CURRENT_BINARY_DIR}/renode_st_tests.yaml
  -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
  TOOLCHAIN
  ${CMAKE_CURRENT_SOURCE_DIR}/toolchains/arm-none-eabi-toolchain.cmake)

add_custom_target(
  run_st
  COMMAND renode-test --debug-on-error -t
          ${CMAKE_CURRENT_BINARY_DIR}/renode_st_tests.yaml
  DEPENDS stm32f0_disco
  VERBATIM)
