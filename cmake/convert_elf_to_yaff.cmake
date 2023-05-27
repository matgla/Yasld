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
function(convert_elf_to_yaff target)
  message(STATUS "Adding conversion step for target: ${target}")
  # set_target_properties(${target} PROPERTIES LINK_FLAGS_RELEASE -s)

  # Strip debug symbols
  #
  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${CMAKE_STRIP} -d $<TARGET_FILE:${target}>
    VERBATIM)
  # ${yasld_python} ${SCRIPTS_DIR}/mkimage.py --input=$<TARGET_FILE:${target}>
  # --output=${target} -v DEPENDS ${target} ${SCRIPTS_DIR}/scripts/mkimage.py
  # VERBATIM)

endfunction()
