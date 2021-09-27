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

message (STATUS "Using api file: ${MSOS_DYNAMIC_LINKER_API_FILE}")

find_package(cmakeutils REQUIRED)
include (virtualenv)

setup_virtualenv(generate_api_venv ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt)

set (GENERATE_API ${CMAKE_CURRENT_SOURCE_DIR}/generate_api.py)
set (python_executable ${CMAKE_CURRENT_BINARY_DIR}/generate_api_venv/bin/python3)
message(STATUS "API generator uses: ${python_executable}")
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json
    COMMAND ${python_executable} ${GENERATE_API} --api=${MSOS_DYNAMIC_LINKER_API_FILE} --output ${CMAKE_CURRENT_BINARY_DIR}/ --input ${PROJECT_BINARY_DIR}
    DEPENDS ${GENERATE_API} ${MSOS_DYNAMIC_LINKER_API_FILE}
    VERBATIM
)

set (msos_dl_api_header_dir ${CMAKE_CURRENT_BINARY_DIR} CACHE INTERNAL "" FORCE)
add_custom_target(api_generator
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h
    VERBATIM
)

set (generated_api_file ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json CACHE INTERNAL "" FORCE)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h PROPERTIES GENERATED 1 )
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json PROPERTIES GENERATED 1 )

set_source_files_properties(${generated_api_file} PROPERTIES GENERATED 1 )


