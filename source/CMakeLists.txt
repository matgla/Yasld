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

add_library(yasld)

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/yasld/)

target_sources(
  yasld
  PUBLIC ${include_dir}/align.hpp
         ${include_dir}/loader.hpp
         ${include_dir}/header.hpp
         ${include_dir}/relocation.hpp
         ${include_dir}/symbol.hpp
         ${include_dir}/symbol_table.hpp
         ${include_dir}/logger.hpp
         ${include_dir}/parser.hpp
         ${include_dir}/relocation_table.hpp
         ${include_dir}/symbol_iterator.hpp
         ${include_dir}/executable.hpp
  PRIVATE loader.cpp
          executable.cpp
          header.cpp
          relocation.cpp
          symbol.cpp
          symbol_table.cpp
          parser.cpp
          relocation_table.cpp
          symbol_iterator.cpp)

add_subdirectory(arch)

find_package(eul REQUIRED)

target_link_libraries(
  yasld
  PUBLIC eul::functional
  PRIVATE yasld_arch yasld_flags)

target_include_directories(yasld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
