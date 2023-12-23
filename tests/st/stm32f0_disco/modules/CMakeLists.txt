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

cmake_minimum_required(VERSION 3.24)

project(
  stm32f0_modules
  C
  CXX
  ASM)

message(STATUS "STM32F0 Discovery Board Test Modules")

include(FetchContent)

FetchContent_Declare(
  yaspem
  GIT_REPOSITORY https://github.com/matgla/Yaspem.git
  GIT_TAG v1.4)

FetchContent_MakeAvailable(yaspem)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${yaspem_SOURCE_DIR}/cmake)

include(yaspem)

setup_yaspem(
  YASPEM_SOURCE
  ${yaspem_SOURCE_DIR}
  OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/packages
  PACKAGE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/packages.json)

set(YASLD_IS_NOT_PARENT ON)
set(YASLD_DISABLE_TESTS ON)
set(YASLD_ONLY_CMAKE ON)
set(yasld_root_path "../../../..")

get_filename_component(YASLD_CMAKE_MODULE_PATH ${yasld_root_path}/cmake
                       ABSOLUTE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/../cmake
                      ${YASLD_CMAKE_MODULE_PATH})

set(YASLD_ARCH armv6-m)

message(STATUS "Adding Yasld target")
add_subdirectory(${yasld_root_path} ${CMAKE_CURRENT_BINARY_DIR}/yasld)

include(CreateLibOpenCM3)

create_libopencm3_with_pic(stm32/f0)

add_library(build_flags_for_shared INTERFACE)

target_compile_options(
  build_flags_for_shared
  INTERFACE -nostartfiles
            -mthumb
            -mfloat-abi=soft
            -Wall
            -Wpedantic
            -Werror
            -Wextra
            -fno-plt
            $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions
            -fno-rtti>)

target_link_options(
  build_flags_for_shared
  INTERFACE
  -nostartfiles
  -fno-plt
  --specs=nano.specs)

target_link_libraries(build_flags_for_shared INTERFACE yasld_arch_flags)

add_library(build_flags INTERFACE)

target_link_options(build_flags INTERFACE -Wl,--undefined,_printf_float)
target_link_libraries(build_flags INTERFACE build_flags_for_shared)

add_subdirectory(common)
# add_subdirectory(hello_world) add_subdirectory(sum)
# add_subdirectory(floating_point) add_subdirectory(hello_world_shared)
# add_subdirectory(sum_of_sine_of_extern_values) add_subdirectory(classes)
add_subdirectory(simple_library)
