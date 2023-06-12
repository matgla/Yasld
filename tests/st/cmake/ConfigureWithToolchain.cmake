#
# ConfigureWithToolchain.cmake
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
  configure_with_toolchain
  name
  toolchain
  args
  depends)
  include(ExternalProject)
  ExternalProject_Add(
    ${name}
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_TOOLCHAIN_FILE=${toolchain} ${args}
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${name}
    BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/${name}
    BUILD_ALWAYS 1
    DEPENDS ${depends}
    INSTALL_COMMAND "")
endfunction()
