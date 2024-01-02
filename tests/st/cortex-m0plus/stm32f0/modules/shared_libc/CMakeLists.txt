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

# Note: Shared library enforces PLT generation on arm-none-eabi which is not
# supported for cortex-m0
#
# Workaround is to treat executable as library
add_executable(stm32f0_shared_libc)

find_package(baselibc REQUIRED)

file(GLOB sources ${baselibc_SOURCE_DIR}/src/*.c)
file(GLOB includes ${baselibc_SOURCE_DIR}/include/**)

target_sources(
  stm32f0_shared_libc
  PUBLIC ${includes}
  PRIVATE ${sources})

target_link_libraries(
  stm32f0_shared_libc PRIVATE test::st::cortex_m0_plus::module_flags
                              yasld_shared_library_flags gcc)

target_include_directories(stm32f0_shared_libc SYSTEM BEFORE
                           PUBLIC ${baselibc_SOURCE_DIR}/include)

target_include_directories(stm32f0_shared_libc
                           PUBLIC ${baselibc_SOURCE_DIR}/src/templates)

target_compile_options(
  stm32f0_shared_libc
  PUBLIC -fvisibility=default
         -Wl,--export-dynamic
         -Wno-array-parameter
         -Wno-unused-parameter)

set_target_properties(stm32f0_shared_libc PROPERTIES C_VISIBILITY_PRESET
                                                     default)
target_link_libraries(stm32f0_shared_libc PUBLIC -fvisibility=default)

include(ConvertElfToYasiff)
convert_elf_to_yasiff(
  TARGET
  stm32f0_shared_libc
  TYPE
  "shared_library")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/stm32f0_shared_libc.yaff
        DESTINATION ${STM32F0_TEST_MODULES_DIR})
