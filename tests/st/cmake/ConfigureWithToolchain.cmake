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
  configure_with_toolchain_impl
  name
  args
  source
  binary
  depends
  install)
  message(STATUS "Configuring project with toolchain")
  include(ExternalProject)
  if(NOT
     "${install}"
     STREQUAL
     "")
    ExternalProject_Add(
      ${name}
      CMAKE_ARGS ${args}
      SOURCE_DIR ${source}
      BINARY_DIR ${binary}
      INSTALL_DIR ${install}
      BUILD_ALWAYS 1
      DEPENDS ${depends})
  else()
    ExternalProject_Add(
      ${name}
      CMAKE_ARGS ${args}
      SOURCE_DIR ${source}
      BINARY_DIR ${binary}
      INSTALL_COMMAND ""
      BUILD_ALWAYS 1
      DEPENDS ${depends})
  endif()

endfunction()

macro(configure_with_toolchain)
  set(prefix CFG)
  set(optionArgs "")
  set(singleValueArgs
      TOOLCHAIN
      NAME
      INSTALL)
  set(mutliValueArgs ARGS DEPENDS)

  include(CMakeParseArguments)
  cmake_parse_arguments(
    ${prefix}
    "${optionArgs}"
    "${singleValueArgs}"
    "${mutliValueArgs}"
    ${ARGN})

  if(NOT
     DEFINED
     CMAKE_BUILD_TYPE
     OR "${CMAKE_BUILD_TYPE}"
        STREQUAL
        "")
    set(CMAKE_BUILD_TYPE "Release")
  endif()

  if(NOT
     DEFINED
     CFG_DEPENDS)
    set(CFG_DEPENDS "")
  endif()

  if(NOT
     DEFINED
     CFG_ARGS)
    set(CFG_ARGS "")
  endif()

  if(NOT
     DEFINED
     CFG_TOOLCHAIN)
    message(FATAL_ERROR "TOOLCHAIN must be passed to configure_with_toolchain")
  endif()

  if(NOT
     DEFINED
     CFG_NAME)
    message(FATAL_ERROR "NAME must be passed to configure_with_toolchain")
  endif()

  message(STATUS "External project name     : ${CFG_NAME}")
  message(STATUS "External project toolchain: ${CFG_TOOLCHAIN}")
  message(STATUS "External project args     : ${CFG_ARGS}")
  message(STATUS "External project depends  : ${CFG_DEPENDS}")
  message(STATUS "External project install  : ${CFG_INSTALL}")

  set(args
      "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE};-DCMAKE_TOOLCHAIN_FILE=${CFG_TOOLCHAIN};${CFG_ARGS};"
  )

  if(DEFINED CFG_INSTALL)
    set(args "${args};-DCMAKE_INSTALL_PREFIX=${CFG_INSTALL};")
  endif()

  configure_with_toolchain_impl(
    ${CFG_NAME}
    "${args}"
    "${CMAKE_CURRENT_SOURCE_DIR}/${CFG_NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/${CFG_NAME}"
    "${CFG_DEPENDS}"
    "${CFG_INSTALL}")
endmacro()
