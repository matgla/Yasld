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

add_library(test_st_cortex_m0_plus_nucleo_f091rc INTERFACE)
add_library(test::st::cortex_m0_plus::nucleo_f091rc ALIAS
            test_st_cortex_m0_plus_nucleo_f091rc)

target_link_libraries(
  test_st_cortex_m0_plus_nucleo_f091rc
  INTERFACE test::st::cortex_m0_plus::stm32f0
            test::st::cortex_m0_plus::host_common)

target_link_options(test_st_cortex_m0_plus_nucleo_f091rc INTERFACE
                    -T${CMAKE_CURRENT_SOURCE_DIR}/../stm32f0_nucleo_f091rc.ld)

add_subdirectory(import_libc_symbols)
add_subdirectory(print_hello_world)
add_subdirectory(print_hello_world_using_external_symbols)
add_subdirectory(executable_with_shared_libraries)
add_subdirectory(floppy_test)
add_subdirectory(correct_static_construction)
