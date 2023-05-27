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
  -T${PROJECT_SOURCE_DIR}/arch/linker_scripts/arm/executable.ld
  -nostartfiles
  -Wl,--emit-relocs
  -Wl,--no-warn-rwx-segments
  -fvisibility-inlines-hidden
  -Wl,--exclude-libs=ALL
  -fvisibility=hidden)
# exceptions are not supported right now
target_compile_options(
  yasld_executable_flags
  INTERFACE -s # strip debug symbols
            -fno-exceptions
            -fvisibility-inlines-hidden
            -fno-section-anchors
            -mpic-register=r9
            -msingle-pic-base
            -mno-pic-data-is-text-relative
            -fPIE
            -mlong-calls
            -fvisibility=hidden)

add_library(yasld_dynamic_link_stdlib INTERFACE)

# TODO: in fact we don't have newlib-libc compiled as shared library so let's
# workaround linker errors with ignore
target_link_options(
  yasld_dynamic_link_stdlib INTERFACE
  -Wl,--unresolved-symbols=ignore-in-object-files -nodefaultlibs -nostdlib)
