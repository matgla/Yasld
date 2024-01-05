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
      $<TARGET_OBJECTS:${target}> --output ${target}_wrappers.s --objcopy
      ${CMAKE_OBJCOPY} --verbose --template ${YASLD_ARCH_DIR}/${YASLD_ARCH}
      --compiler=${CMAKE_C_COMPILER} --ar ${CMAKE_AR}
      --compiler_flags=${yasld_arch_flags_str}
    DEPENDS ${MKIMAGE_DIR}/generate_wrappers.py ${YASLD_ARCH_DIR}/${YASLD_ARCH}/call_wrapped.s.tmpl
    VERBATIM)

  add_custom_command(
    TARGET ${target}
    POST_BUILD
    COMMAND ${mkimage_python_executable} ${MKIMAGE_DIR}/fix_names.py --input
            $<TARGET_FILE:${target}> --objcopy ${CMAKE_OBJCOPY}
    VERBATIM)

  target_link_directories(${target} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(
    ${target} PRIVATE -Wl,--whole-archive ${target}_wrappers.a
                      -Wl,--no-whole-archive)
endfunction()
