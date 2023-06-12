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
  stm32f0_return_value_test
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD
    20)

message(STATUS "STM32F0 tests configuration started")

include(FetchContent)

FetchContent_Declare(
  yaspem
  GIT_REPOSITORY https://github.com/matgla/Yaspem.git
  GIT_TAG v1.2)

FetchContent_MakeAvailable(yaspem)
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${yaspem_SOURCE_DIR}/cmake)

include(yaspem)

setup_yaspem(
  YASPEM_SOURCE
  ${yaspem_SOURCE_DIR}
  OUTPUT_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/packages/
  PACKAGE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/packages.json)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../../cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(../../cmake/BuildFlags.cmake)

include(CreateLibOpenCM3Target)
create_libopencm3_target(stm32/f0)

add_subdirectory(../../../../.. ${CMAKE_CURRENT_BINARY_DIR}/yasld)

add_subdirectory(source)
