# This file is part of MSOS project. This is simple OS for embedded development devices.
# Copyright (C) 2019 Mateusz Stadnik
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

#find_file (GENERATE_API generate_api.py ${PROJECT_SOURCE_DIR}/scripts)

#add_custom_command(
#    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json
#    COMMAND python3 ${GENERATE_API} --api=${PROJECT_SOURCE_DIR}/api/symbol_codes.json --output ${CMAKE_CURRENT_BINARY_DIR}/ --input ${PROJECT_BINARY_DIR}
#    DEPENDS ${GENERATE_API} ${PROJECT_SOURCE_DIR}/api/symbol_codes.json
#    VERBATIM
#)

message (STATUS "Using api file: ${MSOS_DYNAMIC_LINKER_API_FILE}")

message (STATUS "Scripts directory: ${PROJECT_SOURCE_DIR}/scripts")
set (GENERATE_API ${PROJECT_SOURCE_DIR}/scripts/generate_api.py)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json
    COMMAND python3 ${GENERATE_API} --api=${MSOS_DYNAMIC_LINKER_API_FILE} --output ${CMAKE_CURRENT_BINARY_DIR}/ --input ${PROJECT_BINARY_DIR}
    DEPENDS ${GENERATE_API} ${MSOS_DYNAMIC_LINKER_API_FILE}
    VERBATIM
)

add_custom_target(api_generator
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h
    VERBATIM
)

set (generated_api_file ${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json CACHE INTERNAL "" FORCE)

set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h PROPERTIES GENERATED 1 )
set_source_files_properties(${CMAKE_CURRENT_BINARY_DIR}/generated_symbols.json PROPERTIES GENERATED 1 )

set_source_files_properties(${generated_api_file} PROPERTIES GENERATED 1 )

set (library_includes ${PROJECT_SOURCE_DIR}/include/msos/dynamic_linker)

add_subdirectory(arch)

add_library(msos_dynamic_linker STATIC ${CMAKE_CURRENT_BINARY_DIR}/symbol_codes.h)

target_sources(msos_dynamic_linker
    PUBLIC
        ${library_includes}/dynamic_linker.hpp
        ${library_includes}/dynamic_linker_errors.hpp
        ${library_includes}/fwd.hpp
        ${library_includes}/environment.hpp
        ${library_includes}/loaded_module.hpp
        ${library_includes}/module.hpp
        ${library_includes}/module_data.hpp
        ${library_includes}/module_header.hpp
        ${library_includes}/relocation.hpp
        ${library_includes}/symbol.hpp

    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_linker.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_linker_errors.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/loaded_module.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/module.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/module_data.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/module_header.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/relocation.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/symbol.cpp
)

target_include_directories(msos_dynamic_linker
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
)

target_compile_options(msos_dynamic_linker
    PUBLIC
        $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a>
        $<$<CONFIG:DEBUG>:-Og -g>
        $<$<CONFIG:RELEASE>:-Os>
    PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-Werror -Wall -Wextra -Wpedantic -Wconversion -Wunused -Wshadow -Wold-style-cast -Wpointer-arith -Wcast-qual -Wno-missing-braces>
)

find_package(eul REQUIRED)

target_link_libraries(msos_dynamic_linker
    PUBLIC
        eul_error
    PRIVATE 
        msos_dynamic_linker_arch
)
