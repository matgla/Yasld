#
# arm-none-eabi-toolchain.cmake
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

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

if(NOT DEFINED ENV{YASLD_TOOLCHAIN_PATH}
   AND (NOT DEFINED YASLD_TOOLCHAIN_PATH OR "${YASLD_TOOLCHAIN_PATH}" STREQUAL
                                            "")
   AND NOT DEFINED GCC_ROOT_PATH)
  message(
    FATAL_ERROR
      "Please define path to Yasld toolchain via environment variable YASLD_TOOLCHAIN_PATH or pass -DYASLD_TOOLCHAIN_PATH=<path> to cmake"
  )
endif()

if(DEFINED YASLD_TOOLCHAIN_PATH
   AND ${YASLD_TOOLCHAIN_PATH}
       NOT
       STREQUAL
       "")
  set(GCC_ROOT_PATH
      ${YASLD_TOOLCHAIN_PATH}/bin
      CACHE INTERNAL "" FORCE)
elseif(DEFINED ENV{YASLD_TOOLCHAIN_PATH})
  set(GCC_ROOT_PATH
      $ENV{YASLD_TOOLCHAIN_PATH}/bin
      CACHE INTERNAL "" FORCE)
endif()

set(CMAKE_ASM_COMPILER ${GCC_ROOT_PATH}/arm-none-eabi-gcc)
set(CMAKE_C_COMPILER ${GCC_ROOT_PATH}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${GCC_ROOT_PATH}/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${GCC_ROOT_PATH}/arm-none-eabi-objcopy)
set(CMAKE_AR ${GCC_ROOT_PATH}/arm-none-eabi-gcc-ar)
set(CMAKE_RANLIB ${GCC_ROOT_PATH}/arm-none-eabi-ranlib)
set(CMAKE_C_COMPILER_RANLIB ${GCC_ROOT_PATH}/arm-none-eabi-ranlib)
set(CMAKE_SIZE ${GCC_ROOT_PATH}/arm-none-eabi-size)
set(CMAKE_LINKER ${GCC_ROOT_PATH}/arm-none-eabi-ld)

set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
