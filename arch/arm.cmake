#
# arm.cmake
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
# this program. If not, see <h:ttps://www.gnu.org/licenses/>.
#

add_library(yasld_executable_flags INTERFACE)

# TODO: check if linker script may be overriden by client code

add_link_options(-fvisibility=hidden)

target_link_options(
  yasld_executable_flags
  INTERFACE
  -Wl,--emit-relocs
  -Wl,--no-warn-rwx-segments
  -fvisibility-inlines-hidden
  -nostartfiles
  -fvisibility=hidden)

if(NOT DEFINED YASLD_USE_CUSTOM_LINKER_SCRIPT)
  target_link_options(
    yasld_executable_flags INTERFACE
    -T${PROJECT_SOURCE_DIR}/arch/linker_scripts/arm/executable.ld)

endif()
# exceptions are not supported right now
target_compile_options(
  yasld_executable_flags
  INTERFACE # strip debug symbols
            -fno-exceptions
            -fno-rtti
            -fvisibility=hidden
            $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden>
            -mpic-register=r9
            -msingle-pic-base
            -mno-pic-data-is-text-relative
            -fPIC)

add_library(yasld_dynamic_link_stdlib INTERFACE)

# TODO: in fact we don't have newlib-libc compiled as shared library so let's
# workaround linker errors with ignore
target_link_options(
  yasld_dynamic_link_stdlib
  INTERFACE
  -Wl,--unresolved-symbols=ignore-in-object-files
  -nodefaultlibs
  -nostdlib)
