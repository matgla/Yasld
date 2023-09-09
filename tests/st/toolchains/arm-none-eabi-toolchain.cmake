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

find_program(c_compiler NAMES "arm-none-eabi-gcc")
find_program(cxx_compiler NAMES "arm-none-eabi-g++")
find_program(objcopy NAMES "arm-none-eabi-objcopy")
find_program(objdump NAMES "arm-none-eabi-objdump")
find_program(size NAMES "arm-none-eabi-size")
find_program(ar NAMES "arm-none-eabi-gcc-ar")
find_program(ranlib NAMES "arm-none-eabi-ranlib")
find_program(ld NAMES "arm-none-eabi-ld")

set(CMAKE_ASM_COMPILER ${c_compiler})
set(CMAKE_C_COMPILER ${c_compiler})
set(CMAKE_CXX_COMPILER ${cxx_compiler})
set(CMAKE_OBJCOPY ${objcopy})
set(CMAKE_AR ${ar})
set(CMAKE_RANLIB ${ranlib})
set(CMAKE_C_COMPILER_RANLIB ${ranlib})
set(CMAKE_SIZE ${size})
set(CMAKE_LINKER ${linker})

set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
