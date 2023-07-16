#
# ConfigureWithToolchain.cmake
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

function(
  configure_impl
  name
  args
  source
  binary
  depends)
  ExternalProject_Add(
    ${name}
    CMAKE_ARGS ${args}
    SOURCE_DIR ${source}
    BINARY_DIR ${binary}
    BUILD_ALWAYS 1
    DEPENDS ${depends}
    INSTALL_COMMAND "")
endfunction()

macro(configure_with_toolchain)
  set(prefix CFG)
  set(optionArgs "")
  set(singleValueArgs TOOLCHAIN NAME)
  set(multiValueArgs ARGS DEPENDS)

  include(CMakeParseArguments)

  cmake_parse_arguments(
    ${prefix}
    "${optionArgs}"
    "${singleValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  include(ExternalProject)

  if(NOT DEFINED CMAKE_BUILD_TYPE OR "${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Release)
  endif()

  if(NOT DEFINED CFG_DEPENDS)
    set(CFG_DEPENDS "")
  endif()

  set(args
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};-DCMAKE_TOOLCHAIN_FILE=${CFG_TOOLCHAIN};${CFG_ARGS}"
  )

  configure_impl(
    ${CFG_NAME}
    "${args}"
    "${CMAKE_CURRENT_SOURCE_DIR}/${CFG_NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/${CFG_NAME}"
    "${CFG_DEPENDS}")

endmacro()
