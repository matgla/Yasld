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

add_library(msos_dl_public_interface INTERFACE)

target_sources(msos_dl_public_interface 
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/dynamic_linker.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/dynamic_linker_errors.hpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/environment.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/fwd.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/loaded_module.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/logging.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/module.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/module_data.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/module_header.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/relocation.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/msos/dynamic_linker/symbol.hpp
)

target_include_directories(msos_dl_public_interface
    INTERFACE 
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(msos_dl_public_interface
    INTERFACE 
        eul_error
)

        
