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

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../cmake ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(ConfigureWithToolchain)

configure_with_toolchain(
  NAME
  stm32f0
  TOOLCHAIN
  ${CMAKE_CURRENT_SOURCE_DIR}/../toolchains/arm-none-eabi-toolchain.cmake
  ARGS
  -DYASLD_TOOLCHAIN_PATH=${YASLD_TOOLCHAIN_PATH}
  -DYASLD_ARCH_DIR=${PROJECT_SOURCE_DIR}/source/arch
  -Dyaspem_SOURCE_DIR=${yaspem_SOURCE_DIR}
  -Dyaspem_PACKAGES_DIR=${PROJECT_BINARY_DIR}/packages
)

add_custom_target(run_stm32f0_st
  COMMAND renode-test -t
          ${CMAKE_CURRENT_BINARY_DIR}/stm32f0/stm32f0_tests.yaml
  DEPENDS stm32f0
  VERBATIM)

add_test(
  build_tests
  "${CMAKE_COMMAND}"
  --build
  "${CMAKE_BINARY_DIR}"
  --config
  "$<CONFIG>"
  --target
  stm32f0)

set_tests_properties(build_tests PROPERTIES FIXTURES_SETUP stm32f0_st_fixture)

add_test(NAME Stm32F0SystemTests COMMAND ${CMAKE_COMMAND} --build
                                         ${CMAKE_BINARY_DIR} -- run_stm32f0_st)

set_tests_properties(Stm32F0SystemTests PROPERTIES FIXTURES_REQUIRED
                                                   stm32f0_st_fixture)
