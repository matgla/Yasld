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

if (NOT DEFINED TEST_ST_CORTEX_M0_PLUS_HOST_LINKER_SCRIPT OR ${TEST_ST_CORTEX_M0_PLUS_HOST_LINKER_SCRIPT} STREQUAL "")
  message(FATAL_ERROR "Tests host linker script is not defined")
endif ()

add_library(test_st_cortex_m0_plus_host_flags INTERFACE)
add_library(test::st::cortex_m0_plus::host_flags ALIAS test_st_cortex_m0_plus_host_flags)

target_compile_options(
  test_st_cortex_m0_plus_host_flags 
  INTERFACE 
    -mcpu=cortex-m0plus
    -nostartfiles
    -mthumb
    -mfloat-abi=soft
    -Wall 
    -Wpedantic
    -Werror
    -Wextra
    $<$<COMPILE_LANGUAGE:CXX>:
     -fno-exceptions
     -fno-rtti>
)

target_link_options(test_st_cortex_m0_plus_host_flags
  INTERFACE 
    -mcpu=cortex-m0plus
    -nostartfiles
    -mthumb
    -Wl,--undefined,_printf_float
    --specs=nano.specs
    -T${TEST_ST_CORTEX_M0_PLUS_HOST_LINKER_SCRIPT}
)

