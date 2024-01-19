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

cmake_minimum_required(VERSION 3.20)

if(NOT DEFINED YASLD_IS_NOT_PARENT)

  message(STATUS "Yasld configuration started")

  project(yasld NONE)

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

  enable_language(C CXX ASM)

  set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

add_subdirectory(mkimage)

if(NOT DEFINED YASLD_ARCH)
  set(YASLD_ARCH "host")
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/arch/${YASLD_ARCH}/arch.cmake)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(NOT YASLD_ONLY_CMAKE)
  add_definitions("-DLOGGER_ENABLED=0")
  add_library(yasld_flags INTERFACE)
  target_link_libraries(yasld_flags INTERFACE yasld_arch_flags)

  target_compile_options(
    yasld_flags
    INTERFACE -Wall
              -Wextra
              -Werror
              -Wshadow
              -Wdouble-promotion
              -Wno-format
              -Wformat-truncation
              -Wundef
              -Wconversion
              -ffunction-sections
              -fdata-sections
              -Wno-unused-parameter
              $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions
              -fno-rtti>)

  add_subdirectory(source)

  if(YASLD_ENABLE_LOGGER)
    target_compile_definitions(yasld_flags INTERFACE -DLOGGER_ENABLED=1)
  endif()

  if(NOT DEFINED YASLD_DISABLE_TESTS)
    message(STATUS "Adding YASLD tests")
    enable_testing()

    setup_yaspem(
      YASPEM_SOURCE
      ${yaspem_SOURCE_DIR}
      OUTPUT_DIRECTORY
      ${CMAKE_CURRENT_BINARY_DIR}/packages
      PACKAGE_FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/tests/ut/packages.json)

    add_subdirectory(tests)
  endif()

endif()
