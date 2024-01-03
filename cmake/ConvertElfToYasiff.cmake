#
# ConvertElfToYasiff.cmake
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

set(CURRENT_FILE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(MKIMAGE_DIR ${CURRENT_FILE_DIR}/../mkimage)

include(GenerateWrappers)

macro(convert_elf_to_yasiff)
  set(prefix YASIFF)
  set(optionArgs "")
  set(singleValueArgs TARGET TYPE)
  set(multiValueArgs LIBRARIES)

  include(CMakeParseArguments)
  cmake_parse_arguments(
    ${prefix}
    "${optionArgs}"
    "${singleValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  if(${YASIFF_TYPE} STREQUAL "shared_library")
    generate_wrappers_for(${YASIFF_TARGET})
  endif()

  get_filename_component(MKIMAGE_DIR ${MKIMAGE_DIR} ABSOLUTE)

  add_custom_command(
    OUTPUT ${YASIFF_TARGET}.yaff
    COMMAND ${CMAKE_OBJCOPY} --localize-hidden $<TARGET_FILE:${YASIFF_TARGET}>
    COMMAND cmake -E copy $<TARGET_FILE:${YASIFF_TARGET}>
            $<TARGET_FILE:${YASIFF_TARGET}>.bak
    COMMAND ${CMAKE_STRIP} -d $<TARGET_FILE:${YASIFF_TARGET}>
    COMMAND
      ${mkimage_python_executable} ${MKIMAGE_DIR}/mkimage.py
      --type=${YASIFF_TYPE} --input=$<TARGET_FILE:${YASIFF_TARGET}>
      --output=${CMAKE_CURRENT_BINARY_DIR}/${YASIFF_TARGET}.yaff --libraries
      ${YASIFF_LIBRARIES} --verbose
    VERBATIM
    DEPENDS ${MKIMAGE_DIR}/mkimage.py ${YASIFF_TARGET} ${YASIFF_LIBRARIES}
    COMMENT "Generating YASIFF image for module ${YASIFF_TARGET}")

  add_custom_target(generate_${YASIFF_TARGET}.yaff ALL
                    DEPENDS ${YASIFF_TARGET}.yaff)
endmacro()
