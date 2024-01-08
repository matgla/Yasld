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

# Created for Youtube EP #11
add_executable(stm32f0_cat_spawner)

target_sources(
  stm32f0_cat_spawner
  PUBLIC spawn_cat.hpp
  PRIVATE spawn_cat.cpp)

target_link_libraries(
  stm32f0_cat_spawner PRIVATE test::st::cortex_m0_plus::module_flags
                              yasld_executable_flags gcc)

include(ConvertElfToYasiff)
convert_elf_to_yasiff(
  TARGET
  stm32f0_cat_spawner
  TYPE
  shared_library)

target_include_directories(stm32f0_cat_spawner
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_cat_spawner.yaff
        DESTINATION ${STM32F0_TEST_MODULES_DIR})
