# This file is part of msgpu project.
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

cmake_minimum_required(VERSION 3.10)

project(msos_dl CXX)

include(FetchContent)

FetchContent_Declare(
  mspkg
  GIT_REPOSITORY https://github.com/matgla/mspkg.git
  GIT_TAG        master
)

FetchContent_MakeAvailable(mspkg)

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${mspkg_SOURCE_DIR}/cmake)
include (mspkg)

setup_mspkg(${mspkg_SOURCE_DIR})

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)

set (MSOS_DYNAMIC_LINKER_ROOT ${PROJECT_SOURCE_DIR} CACHE INTERNAL "")
set (MSOS_DYNAMIC_LINKER_BINARY_ROOT ${PROJECT_BINARY_DIR} CACHE INTERNAL "")

if (${MSOS_DL_ARCH} STREQUAL "x86_64-linux")
set (MSOS_DYNAMIC_LINKER_API_FILE ${PROJECT_SOURCE_DIR}/stub/linux/symbol_codes.json)
endif ()

add_subdirectory(include)
add_subdirectory(api_generator)

if (${MSOS_DL_ARCH} STREQUAL "x86_64-linux")
    add_subdirectory(stub)
    add_subdirectory(tests)
    set (arch "x86")
else ()
    add_subdirectory(source)
endif ()

if (PARENT_SCOPE)
  set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake PARENT_SCOPE)
endif ()

include (cmake/ModuleWrapperGenerator.cmake)

add_module_flags_target()



