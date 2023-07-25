#
# CreateLibOpenCM3Target.cmake
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

function(create_libopencm3_target target)

  find_package(libopencm3 REQUIRED)

  message(STATUS "Current binary dir: ${CMAKE_CURRENT_BINARY_DIR}")
  file(COPY ${libopencm3_SOURCE_DIR}
       DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic)

  message(
    STATUS
      "Copying ${libopencm3_SOURCE_DIR} to ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic"
  )
  string(
    REPLACE "/"
            ""
            library_name
            ${target})

  add_custom_target(
    ${library_name}_build
    COMMAND make TARGETS=${target}
    WORKING_DIRECTORY ${libopencm3_SOURCE_DIR})

  add_library(${library_name} STATIC IMPORTED)

  set_target_properties(
    ${library_name}
    PROPERTIES IMPORTED_LOCATION
               ${libopencm3_SOURCE_DIR}/lib/libopencm3_${library_name}.a)

  target_include_directories(${library_name}
                             INTERFACE ${libopencm3_SOURCE_DIR}/include)

  add_dependencies(${library_name} ${library_name}_build)

  string(TOUPPER ${library_name} target_define)

  target_compile_definitions(${library_name} INTERFACE -D${target_define})

  target_link_directories(${library_name} INTERFACE
                          ${libopencm3_SOURCE_DIR}/lib)

  # PIC library
  add_custom_target(
    ${library_name}_build_with_pic
    COMMAND
      ${CMAKE_COMMAND} -E env "CFLAGS=-fPIC -mno-pic-data-is-text-relative -g"
      -- make TARGETS=${target}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3
    VERBATIM)

  add_custom_command(
    TARGET ${library_name}_build_with_pic
    POST_BUILD
    COMMAND
      ${CMAKE_COMMAND} -E copy_if_different
      ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3/lib/libopencm3_${library_name}.a
      ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3/lib/libopencm3_${library_name}_with_pic.a
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3
    VERBATIM)

  add_library(${library_name}_with_pic STATIC IMPORTED)

  set_target_properties(
    ${library_name}_with_pic
    PROPERTIES
      IMPORTED_LOCATION
      ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3/lib/libopencm3_${library_name}_with_pic.a
  )

  target_include_directories(
    ${library_name}_with_pic
    INTERFACE ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3/include)

  add_dependencies(${library_name}_with_pic ${library_name}_build_with_pic)

  target_compile_definitions(${library_name}_with_pic
                             INTERFACE -D${target_define})

  target_link_directories(
    ${library_name}_with_pic INTERFACE
    ${CMAKE_CURRENT_BINARY_DIR}/libopencm3_pic/libopencm3/lib)

endfunction()
