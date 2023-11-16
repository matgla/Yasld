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

add_executable(stm32f0_classes_shared)

target_sources(stm32f0_classes_shared PRIVATE main.cpp)

target_link_libraries(
  stm32f0_classes_shared PRIVATE build_flags_for_shared
                                 yasld_executable_flags_shared)

include(ConvertElfToYasiff)
convert_elf_to_yasiff(stm32f0_classes_shared)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_classes_shared.yaff
        DESTINATION ${STM32F0_TEST_MODULES_DIR})
