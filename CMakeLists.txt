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

cmake_minimum_required(VERSION 3.10)

message(STATUS "Yasld configuration started")

# NONE temporary
project(yasld NONE)

if(NOT DEFINED YASLD_ARCH)
  set(YASLD_ARCH "host")
endif()

include(FetchContent)

FetchContent_Declare(
  yaspem
  GIT_REPOSITORY https://github.com/matgla/Yaspem.git
  GIT_TAG v1.2)

FetchContent_MakeAvailable(yaspem)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${yaspem_SOURCE_DIR}/cmake)

include(yaspem)

setup_yaspem(
  YASPEM_SOURCE
  ${yaspem_SOURCE_DIR}
  OUTPUT_DIRECTORY
  ${CMAKE_CURRENT_BINARY_DIR}/packages/
  PACKAGE_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/packages.json)

find_package(CMakeUtils REQUIRED)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

if(NOT PROJECT_IS_TOP_LEVEL)
  set(CMAKE_MODULE_PATH
      ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake
      PARENT_SCOPE)
endif()

include(virtualenv)
create_virtualenv(yasld ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt
                  ${CMAKE_CURRENT_BINARY_DIR}/venvs)

create_virtualenv(scripts ${CMAKE_CURRENT_SOURCE_DIR}/scripts/requirements.txt
                  ${CMAKE_CURRENT_BINARY_DIR}/venvs)

enable_language(CXX ASM C)

add_library(yasld_flags INTERFACE)

target_compile_options(
  yasld_flags
  INTERFACE $<$<COMPILE_LANGUAGE:CXX>:${YASLD_CXX_FLAGS}>
            $<$<COMPILE_LANGUAGE:C>:${YASLD_C_FLAGS}>
            $<$<COMPILE_LANGUAGE:ASM>:${YASLD_ASM_FLAGS}>)

target_link_options(yasld_flags INTERFACE ${YASLD_LINKER_FLAGS})

add_definitions(-DLOGGER_ENABLED=1)

set(CMAKE_CXX_STANDARD 20)

if(YASLD_ARCH STREQUAL "armv6-m")
  include(arch/arm.cmake)
endif()

add_subdirectory(source)

if(NOT DEFINED YASLD_DISABLE_TESTS)
  enable_testing()
  message(STATUS "Adding tests")
  add_subdirectory(tests)
endif()
