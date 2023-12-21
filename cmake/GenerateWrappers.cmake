#
# GenerateWrappers.cmake
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
set(YASLD_ARCH_PATH ${YASLD_ARCH_PATH})

function(generate_wrappers_for target)
  add_custom_command(
    TARGET ${target}
    PRE_LINK
    COMMAND
      ${mkimage_python_executable} ${MKIMAGE_DIR}/generate_wrappers.py --input
      $<TARGET_OBJECTS:${target}> --output ${target}_wrappers.S --objcopy
      ${CMAKE_OBJCOPY} --verbose --template ${YASLD_ARCH_DIR}/${YASLD_ARCH}
      --compiler=${CMAKE_ASM_COMPILER} --ar ${CMAKE_AR}
      --compiler_flags=${yasld_arch_flags_str}
      ${MKIMAGE_DIR}/generate_wrappers.py
    VERBATIM)

  # add_library(${target}_wrapped OBJECT IMPORTED)
  #
  # set_property( TARGET ${target}_wrapped PROPERTY IMPORTED_OBJECTS
  # ${CMAKE_CURRENT_BINARY_DIR}/${target}_wrappers.S.obj)

  target_link_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(${target} PRIVATE ${target}_wrappers.a)
endfunction()
