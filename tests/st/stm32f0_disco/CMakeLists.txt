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
  stm32f0_tests
  LANGUAGES CXX
            C
            ASM)

message(STATUS "Stm32F0 Discovery Board Testing Project configuration started")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

message(STATUS "Yaspem directory is: ${YASPEM_PATH}")

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/../cmake
    ${MODULES_PATH})

set(yasld_root "../../../")
get_filename_component(
  yasld_root
  ${yasld_root}
  ABSOLUTE)

include(FetchContent)

FetchContent_Declare(
  yaspem
  GIT_REPOSITORY https://github.com/matgla/Yaspem.git
  GIT_TAG v1.4)

FetchContent_MakeAvailable(yaspem)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${yaspem_SOURCE_DIR}/cmake)

include(yaspem)
message(
  STATUS
    "--------------------------------------------------- ${yaspem_SOURCE_DIR}")
setup_yaspem(
  YASPEM_SOURCE ${yaspem_SOURCE_DIR}
  OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/packages
  PACKAGE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/packages.json
                ${yasld_root}/packages.json)

find_package(CMakeUtils REQUIRED)

include(virtualenv REQUIRED)
list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)

include(CreateLibOpenCM3)

create_libopencm3(stm32/f0)

set(ST_TESTS_FILE ${PROJECT_BINARY_DIR}/stm32f0_tests.yaml)
set(STM32F0_TEST_MODULES_DIR ${CMAKE_CURRENT_BINARY_DIR}/modules)

include(BuildFlags)

set(YASLD_IS_NOT_PARENT ON)
set(YASLD_DISABLE_TESTS ON)
set(YASLD_ENABLE_LOGGER ON)
set(YASLD_ARCH armv6-m)

add_subdirectory(${yasld_root} ${CMAKE_CURRENT_BINARY_DIR}/yasld)

include(ConfigureWithToolchain)
configure_with_toolchain(
  NAME modules
  TOOLCHAIN
    ${PROJECT_SOURCE_DIR}/../toolchains/arm-none-eabi-toolchain-with-pic.cmake
  ARGS -DYASLD_TOOLCHAIN_PATH=${YASLD_TOOLCHAIN_PATH}
       -DSTM32F0_TEST_MODULES_DIR=${STM32F0_TEST_MODULES_DIR}
  INSTALL ${STM32F0_TEST_MODULES_DIR})

add_subdirectory(common)
add_subdirectory(tests)
