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

add_executable(stm32f0_calculate_floating_point_host)

target_sources(stm32f0_calculate_floating_point_host PRIVATE main.cpp)

target_link_libraries(
  stm32f0_calculate_floating_point_host
  PRIVATE stm32f0
          build_flags
          yasld
          stm32f0_host_common)

target_link_options(
  stm32f0_calculate_floating_point_host
  PRIVATE
  -T
  ${PROJECT_SOURCE_DIR}/stm32f0_discovery.ld)

set_source_files_properties(
  ${STM32F0_TEST_MODULES_DIR}/stm32f0_floating_point.yaff PROPERTIES GENERATED
                                                                     TRUE)

set_property(
  DIRECTORY
  APPEND
  PROPERTY CMAKE_CONFIGURE_DEPENDS
           ${STM32F0_TEST_MODULES_DIR}/stm32f0_floating_point.yaff)

set_source_files_properties(
  ${STM32F0_TEST_MODULES_DIR}/stm32f0_floating_point.yaff PROPERTIES GENERATED
                                                                     TRUE)

add_custom_command(
  TARGET stm32f0_calculate_floating_point_host
  POST_BUILD
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:stm32f0_calculate_floating_point_host>
  COMMAND
    ${CMAKE_OBJCOPY} -O binary
    $<TARGET_FILE:stm32f0_calculate_floating_point_host>
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point_host.bin
  COMMAND
    truncate --size=64K
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point_host.bin
    DEPENDS modules)

add_dependencies(stm32f0_calculate_floating_point_host modules)

add_custom_target(
  stm32f0_calculate_floating_point_image ALL
  COMMAND
    cat ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point_host.bin
    ${STM32F0_TEST_MODULES_DIR}/stm32f0_floating_point.yaff >
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point.bin
  DEPENDS modules ${STM32F0_TEST_MODULES_DIR}/stm32f0_floating_point.yaff
          stm32f0_calculate_floating_point_host)

set(renode_test_binary
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point.bin)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/stm32f0_calculate_floating_point_test.robot
  ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point_test.robot @ONLY)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/execute_with_gdb.resc
               ${CMAKE_CURRENT_BINARY_DIR}/execute_with_gdb.resc)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/execute.resc
               ${CMAKE_CURRENT_BINARY_DIR}/execute.resc)

include(RegisterTest)
register_st(
  ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_calculate_floating_point_test.robot)
