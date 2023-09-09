#
# arch.cmake
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

add_library(yasld_executable_flags INTERFACE)

target_link_options(
  yasld_executable_flags
  INTERFACE
  -Wl,--emit-relocs
  -Wl,--no-warn-rwx-segments
  -fvisibility-inlines-hidden
  -nostartfiles
  -fno-inline
  -mno-pic-data-is-text-relative
  -msingle-pic-base
  -mpic-register=r9
  -fomit-frame-pointer
  -fvisibility=hidden)

if(NOT
   DEFINED
   YASLD_USE_CUSTOM_LINKER_SCRIPT)
  target_link_options(
    yasld_executable_flags
    INTERFACE
    -T${CMAKE_CURRENT_LIST_DIR}/executable.ld)
endif()

set_target_properties(
  yasld_executable_flags PROPERTIES INTERFACE_LINK_DEPENDS
                                    ${CMAKE_CURRENT_LIST_DIR}/executable.ld)

target_compile_options(
  yasld_executable_flags
  INTERFACE -fomit-frame-pointer
            -fvisibility=hidden
            $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden
            -fno-rtti
            -fno-exceptions>
            -fno-inline
            -mpic-register=r9
            -msingle-pic-base
            -mno-pic-data-is-text-relative
            -fPIC)

add_library(yasld_arch_flags INTERFACE)

target_compile_options(
  yasld_arch_flags
  INTERFACE -mcpu=cortex-m0
            -mfloat-abi=soft
            -mthumb
            $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions
            -fno-rtti>)

target_link_options(
  yasld_arch_flags
  INTERFACE
  -mcpu=cortex-m0
  -mfloat-abi=soft
  -mthumb)
