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

if(NOT TARGET build_flags)
  set(YASLD_ASM_FLAGS
      -mcpu=cortex-m0
      -fno-common
      -nostartfiles
      -mthumb
      -mfloat-abi=soft
      -fstrict-aliasing
      -O2
      CACHE INTERNAL "")

  set(YASLD_C_FLAGS
      ${YASLD_ASM_FLAGS}
      CACHE INTERNAL "")

  set(YASLD_CXX_FLAGS
      ${YASLD_C_FLAGS}
      -fno-exceptions
      -fno-rtti
      -std=c++20
      CACHE INTERNAL "")

  set(YASLD_LINKER_FLAGS
      -mcpu=cortex-m0
      -fno-common
      -nostartfiles
      -mthumb
      # --specs=nano.specs
      -Wl,--undefined,_printf_float
      CACHE INTERNAL "")

  set(YASLD_ARCH
      "armv6-m"
      CACHE INTERNAL "")
  set(YASLD_DISABLE_TESTS
      true
      CACHE INTERNAL "
      ")

  set(CMAKE_ASM_FLAGS
      "${CMAKE_ASM_FLAGS} -mcpu=cortex-m0 -fno-common -nostartfiles -mthumb -mfloat-abi=soft -fstrict-aliasing"
  )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_ASM_FLAGS}")
  set(CMAKE_C_FLAGS
      "${CMAKE_C_FLAGS} ${CMAKE_ASM_FLAGS} -fno-exceptions -fno-rtti -std=c++20"
  )

  add_library(build_flags INTERFACE)
  target_compile_options(build_flags INTERFACE ${YASLD_CXX_FLAGS})
  target_link_options(
    build_flags
    INTERFACE
    ${YASLD_LINKER_FLAGS}
    --specs=nano.specs)
  # )
  add_library(build_flags_host INTERFACE)
  target_compile_options(build_flags_host INTERFACE ${YASLD_CXX_FLAGS})
  target_link_options(build_flags_host INTERFACE ${YASLD_LINKER_FLAGS})
  target_link_libraries(build_flags_host INTERFACE printf)
endif()
