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

add_library(stm32f0_common)

target_sources(
  stm32f0_common
  PUBLIC stub.cpp)

target_compile_options(
  stm32f0_common
  PRIVATE -fPIC
          -mpic-register=r9
          -msingle-pic-base
          -mno-pic-data-is-text-relative)

target_link_libraries(
  stm32f0_common
  PRIVATE build_flags stm32f0_with_pic)
