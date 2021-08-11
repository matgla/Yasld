# This file is part of MSOS project.
# Copyright (C) 2021 Mateusz Stadnik
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

add_library(msos_dynamic_linker STATIC) 

add_dependencies(msos_dynamic_linker api_generator)

target_sources(msos_dynamic_linker 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_linker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/loaded_module.cpp
)

target_include_directories(msos_dynamic_linker 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${msos_dl_api_header_dir}
)

find_package(eul REQUIRED)
target_link_libraries(msos_dynamic_linker 
    PUBLIC 
        msos_dl_public_interface
    PRIVATE 
        eul_utils
)
