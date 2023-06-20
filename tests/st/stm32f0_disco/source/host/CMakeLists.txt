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

add_executable(stm32f0_host)

target_sources(
  stm32f0_host
  PRIVATE main.cpp system_stubs.cpp)

target_link_libraries(
  stm32f0_host
  PRIVATE stm32f0 build_flags yasld)

target_link_options(
  stm32f0_host
  PRIVATE
  -T
  ${PROJECT_SOURCE_DIR}/stm32f0_discovery.ld)

find_program(renode_executable renode REQUIRED)

set(stm32f0_host_binary
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_host.bin)
configure_file(../execute.resc.templ ${CMAKE_CURRENT_BINARY_DIR}/execute.resc)

add_custom_target(
  stm32f0_host_run
  COMMAND ${renode_executable} ${CMAKE_CURRENT_BINARY_DIR}/execute.resc
  DEPENDS stm32f0_host)

set(STM32F0_HOST_BINARY
    ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_host.bin
    CACHE INTERNAL "")

add_custom_command(
  TARGET stm32f0_host
  POST_BUILD
  COMMAND ${CMAKE_SIZE} $<TARGET_FILE:stm32f0_host>
  COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:stm32f0_host>
          ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_host.bin
  COMMAND truncate --size=64K ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_host.bin)

add_custom_target(test)
