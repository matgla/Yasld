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

set(include_dir ${CMAKE_CURRENT_SOURCE_DIR}/include/yasld)

get_target_property(eul_cxx_compilation_flags yasld_arch_flags
                    INTERFACE_COMPILE_OPTIONS)

get_target_property(eul_cxx_linking_flags yasld_arch_flags
                    INTERFACE_LINK_OPTIONS)

if(NOT eul_cxx_compilation_flags)
  set(eul_cxx_compilation_flags "")
endif()

if(NOT eul_cxx_linking_flags)
  set(eul_cxx_linking_flags)
endif()

message(STATUS "${eul_cxx_linking_flags}")

find_package(eul REQUIRED)

target_sources(
  yasld
  PUBLIC ${include_dir}/align.hpp
         ${include_dir}/data_relocation.hpp
         ${include_dir}/environment.hpp
         ${include_dir}/executable.hpp
         ${include_dir}/header.hpp
         ${include_dir}/loader.hpp
         ${include_dir}/local_relocation.hpp
         ${include_dir}/logger.hpp
         ${include_dir}/parser.hpp
         ${include_dir}/relocation.hpp
         ${include_dir}/relocation_table.hpp
         ${include_dir}/section.hpp
         ${include_dir}/symbol.hpp
         ${include_dir}/symbol_iterator.hpp
         ${include_dir}/symbol_table.hpp
  PRIVATE data_relocation.cpp
          executable.cpp
          header.cpp
          loader.cpp
          local_relocation.cpp
          parser.cpp
          relocation.cpp
          section.cpp
          symbol.cpp
          symbol_iterator.cpp
          symbol_table.cpp)

target_include_directories(yasld PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(arch)

target_link_libraries(
  yasld
  PRIVATE yasld_flags yasld_arch
  PUBLIC eul::functional)
