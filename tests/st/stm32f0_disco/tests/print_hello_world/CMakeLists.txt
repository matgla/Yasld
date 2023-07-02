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

add_executable(stm32f0_print_hello_world_host)

target_sources(stm32f0_print_hello_world_host PRIVATE main.cpp)

target_link_libraries(
  stm32f0_print_hello_world_host
  PRIVATE stm32f0
          build_flags
          yasld
          stm32f0_host_common)

target_link_options(
  stm32f0_print_hello_world_host
  PRIVATE
  -T
  ${PROJECT_SOURCE_DIR}/stm32f0_discovery.ld)

add_custom_command(
  TARGET stm32f0_print_hello_world_host
  POST_BUILD
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:stm32f0_print_hello_world_host>
  COMMAND
    ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:stm32f0_print_hello_world_host>
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world_host.bin
  COMMAND truncate --size=64K
          ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world_host.bin
  COMMAND
    cat ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world_host.bin
    ${STM32F0_TEST_MODULES_DIR}/stm32f0_hello_world_standalone.yaff >
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world.bin DEPENDS modules)

set(renode_test_binary
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world.bin)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0_print_hello_world_test.robot
  ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world_test.robot @ONLY)

include(RegisterTest)
register_st(${CMAKE_CURRENT_BINARY_DIR}/stm32f0_print_hello_world_test.robot)
