#
# BuildFlags.cmake
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

if(NOT TARGET build_flags_host)
  add_library(build_flags_host INTERFACE)

  set(ASM_FLAGS
      -mcpu=cortex-m0
      -nostartfiles
      -mthumb
      -mfloat-abi=soft
      -Wall
      -Wpedantic
      -Werror
      -Wextra)

  set(C_FLAGS ${ASM_FLAGS})

  target_compile_options(
    build_flags_host
    INTERFACE ${ASM_FLAGS}
              $<$<COMPILE_LANGUAGE:CXX>:
              -fno-exceptions
              -fno-rtti>)

  target_link_options(
    build_flags_host
    INTERFACE
    -mcpu=cortex-m0
    -nostartfiles
    -mthumb
    -Wl,--undefined,_printf_float
    --specs=nano.specs)
    #-T${PROJECT_SOURCE_DIR}/stm32f0_discovery.ld)

endif()
