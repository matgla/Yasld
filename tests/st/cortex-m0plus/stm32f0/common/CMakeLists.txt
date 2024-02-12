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

add_library(test_st_cortex_m0_plus_host_common)
add_library(test::st::cortex_m0_plus::host_common ALIAS
            test_st_cortex_m0_plus_host_common)

target_sources(
  test_st_cortex_m0_plus_host_common
  PUBLIC include/board_init.hpp system_stubs.cpp
  PRIVATE board_init.cpp)

target_link_libraries(
  test_st_cortex_m0_plus_host_common PUBLIC test::st::cortex_m0_plus::host_flags
                                            test::st::cortex_m0_plus::stm32f0)

target_include_directories(test_st_cortex_m0_plus_host_common
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
