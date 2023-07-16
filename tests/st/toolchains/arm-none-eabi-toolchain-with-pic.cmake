#
# arm-none-eabi-toolchain-with-pic.cmake
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

set(ARM_GCC_PATH_WITH_PIC /opt/arm-none-eabi-with-pic/bin)

set(CMAKE_ASM_COMPILER ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-gcc)
set(CMAKE_C_COMPILER ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-g++)
set(CMAKE_OBJCOPY ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-objdump)
set(CMAKE_AR ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-ar)
set(CMAKE_RANLIB ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-ranlib)
set(CMAKE_C_COMPILER_RANLIB ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-ranlib)
set(CMAKE_SIZE ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-size)
set(CMAKE_LINKER ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-ld)
set(CMAKE_EXE_LINKER ${ARM_GCC_PATH_WITH_PIC}/arm-none-eabi-gcc)

set(CMAKE_EXECUTABLE_SUFFIX_C .elf)
set(CMAKE_EXECUTABLE_SUFFIX_CXX .elf)
set(CMAKE_EXECUTABLE_SUFFIX_ASM .elf)

set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM ONLY)
