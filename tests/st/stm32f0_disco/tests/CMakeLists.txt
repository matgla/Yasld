#
# CMakeLists.txt
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

macro(add_test_and_link_module) 
  set (prefix TEST)
  set (optionArgs "")
  set (singleValueArgs NAME)
  set (multiValueArgs SOURCES LIBRARIES)

  include (CMakeParseArguments)

  cmake_parse_arguments(

    )
  add_executable(${name})

  target_sources(${name} PRIVATE ${sources})
endfunction()

add_subdirectory(common)
add_subdirectory(print_hello_world)
add_subdirectory(calculate_floating_point)
