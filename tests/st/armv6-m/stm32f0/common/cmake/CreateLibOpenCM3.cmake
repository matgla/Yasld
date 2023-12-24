#
# CreateLibOpenCM3.cmake
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
  build_libopencm3
  target
  target_suffix
  path
  flags)

  string(
    REPLACE "/"
            ""
            library_name
            ${target})

  if("${target_suffix}"
     STREQUAL
     "")
    set(target_name ${library_name})
  else()
    set(target_name ${library_name}_${target_suffix})
  endif()

  message(STATUS "Building ${target_name} in: ${path}")

  execute_process(COMMAND ${CMAKE_COMMAND} -E env "CFLAGS=${flags}" -- make
                          TARGETS=${target} WORKING_DIRECTORY ${path})

  add_library(
    ${target_name}
    STATIC
    IMPORTED)

  string(TOUPPER ${library_name} target_define)

  set_target_properties(
    ${target_name}
    PROPERTIES IMPORTED_LOCATION ${path}/lib/libopencm3_${library_name}.a
               INTERFACE_INCLUDE_DIRECTORIES ${path}/include
               INTERFACE_COMPILE_DEFINITIONS "-D${target_define}"
               INTERFACE_LINK_DIRECTORIES ${path}/lib)

  add_dependencies(${target_name} ${target_name}_build)

endfunction()

function(create_libopencm3 target)
  find_package(libopencm3 REQUIRED)
  build_libopencm3(
    ${target}
    ""
    ${libopencm3_SOURCE_DIR}
    "-g -O2")
endfunction()

function(create_libopencm3_with_pic target)
  find_package(libopencm3 REQUIRED)
  build_libopencm3(
    ${target}
    "with_pic"
    ${libopencm3_SOURCE_DIR}
    "-fPIC -mpic-register=r9 -mno-pic-data-is-text-relative -msingle-pic-base -g -O2"
  )
endfunction()
