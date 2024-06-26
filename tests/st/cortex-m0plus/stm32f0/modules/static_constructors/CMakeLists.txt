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

add_executable(stm32f0_static_constructors)

target_sources(stm32f0_static_constructors PRIVATE main.cpp entry.S)

target_link_libraries(
  stm32f0_static_constructors PRIVATE test::st::cortex_m0_plus::stm32f0::modules_common
                      yasld_standalone_executable_flags)

target_link_options(stm32f0_static_constructors 
  PUBLIC 
    -Wl,--entry=_entry
)

include(ConvertElfToYasiff)
convert_elf_to_yasiff(
  TARGET
  stm32f0_static_constructors
  TYPE
  "executable")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_static_constructors.yaff
        DESTINATION ${STM32F0_TEST_MODULES_DIR})
