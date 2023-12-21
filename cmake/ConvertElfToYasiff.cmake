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

function(convert_elf_to_yasiff target)
  get_target_property(target_type ${target} TYPE)

  if(NOT
     ${target_type}
     STREQUAL
     "EXECUTABLE")
    generate_wrappers_for(${target})
  endif()

  get_filename_component(MKIMAGE_DIR ${MKIMAGE_DIR} ABSOLUTE)

  add_custom_command(
    OUTPUT ${target}.yaff
    COMMAND ${CMAKE_OBJCOPY} --localize-hidden $<TARGET_FILE:${target}>
    COMMAND cmake -E copy $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.bak
    COMMAND ${CMAKE_STRIP} -d $<TARGET_FILE:${target}>
    COMMAND
      ${mkimage_python_executable} ${MKIMAGE_DIR}/mkimage.py
      --input=$<TARGET_FILE:${target}>
      --output=${CMAKE_CURRENT_BINARY_DIR}/${target}.yaff --verbose
    VERBATIM
    DEPENDS ${MKIMAGE_DIR}/mkimage.py ${target}
    COMMENT "Generating YASIFF image for module ${target}")

  add_custom_target(generate_${target}.yaff ALL DEPENDS ${target}.yaff)
endfunction()
