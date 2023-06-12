#
# AddTestExecutable.cmake
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

function(add_test_executable name sources)
  add_executable(${name})

  target_sources(
    ${name}
    PRIVATE ${sources})

  include(ConvertElfToYasiff)
  target_link_libraries(
    ${name}
    PRIVATE stm32f0_common
            stm32f0_with_pic
            yasld_executable_flags
            build_flags)

  add_custom_command(
    TARGET ${name}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${name}>)

  convert_elf_to_yasiff(${name})

  add_custom_command(
    TARGET ${name}
    POST_BUILD
    COMMAND
      cat ${HOST_BINARY} ${CMAKE_CURRENT_BINARY_DIR}/${name} >
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_with_host.bin DEPENDS
      ${STM32F0_HOST_BINARY} ${CMAKE_CURRENT_BINARY_DIR}/${name}.bin)

  set(stm32f0_host_binary
      ${CMAKE_CURRENT_BINARY_DIR}/${name}_with_host.bin)

  configure_file(../../../execute.resc.templ
                 ${CMAKE_CURRENT_BINARY_DIR}/execute.resc)

  configure_file(../../../execute_gdb.resc.templ
                 ${CMAKE_CURRENT_BINARY_DIR}/execute_gdb.resc)

  find_program(renode_executable renode REQUIRED)

  add_custom_target(
    ${name}_run
    COMMAND ${renode_executable} ${CMAKE_CURRENT_BINARY_DIR}/execute.resc
    DEPENDS ${name})
endfunction()
