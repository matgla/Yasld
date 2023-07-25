#
# ConvertElfToYaff.cmake
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
set(SCRIPTS_DIR ${CURRENT_FILE_DIR}/../scripts)

function(convert_elf_to_yasiff target)
  message(STATUS "Adding conversion step for target: ${target}")

  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} --localize-hidden $<TARGET_FILE:${target}>
    COMMAND cmake -E copy $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.bak
    COMMAND ${CMAKE_STRIP} -d $<TARGET_FILE:${target}>
    COMMAND
      ${scripts_python_executable} ${SCRIPTS_DIR}/mkimage.py
      --input=$<TARGET_FILE:${target}> --output=${target}.yaff DEPENDS ${target}
      ${SCRIPTS_DIR}/scripts/mkimage.py
    VERBATIM DEPENDS ${SCRIPTS_DIR}/scripts/mkimage.py)

  set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS ${SCRIPTS_DIR}/scripts/mkimage.py)

endfunction()
