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

set(STM32F0_TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

macro(add_test_and_link_module)
  set(prefix TEST)
  set(optionArgs "")
  set(singleValueArgs NAME MODULE)
  set(multiValueArgs SOURCES LIBRARIES)

  include(CMakeParseArguments)

  cmake_parse_arguments(
    ${prefix}
    "${optionArgs}"
    "${singleValueArgs}"
    "${multiValueArgs}"
    ${ARGN})
  add_executable(${TEST_NAME})

  target_sources(${TEST_NAME} PRIVATE ${TEST_SOURCES})
  target_link_libraries(
    ${TEST_NAME}
    PRIVATE stm32f0
            build_flags_host
            yasld
            stm32f0_host_common
            ${TEST_LIBRARIES})

  target_link_options(
    ${TEST_NAME}
    PRIVATE
    -T
    ${PROJECT_SOURCE_DIR}/stm32f0_discovery.ld)

  add_custom_command(
    TARGET ${TEST_NAME}
    POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TEST_NAME}>
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TEST_NAME}>
            ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin
    COMMAND truncate --size=64K ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin)

  add_dependencies(${TEST_NAME} modules)
  add_executable(${TEST_MODULE}.yaff IMPORTED)
  set(MODULE_PATH ${STM32F0_TEST_MODULES_DIR}/${TEST_MODULE}.yaff)
  set_property(TARGET ${TEST_MODULE}.yaff PROPERTY IMPORTED_LOCATION
                                                   ${MODULE_PATH})
  set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS ${MODULE_PATH})

  add_custom_target(
    ${TEST_NAME}_image ALL
    COMMAND cat ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin ${MODULE_PATH} >
            ${TEST_NAME}_test.bin
    DEPENDS ${TEST_NAME} ${TEST_MODULE}.yaff)

  set(renode_test_binary ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_test.bin)
  configure_file(${STM32F0_TESTS_DIR}/execute_with_gdb.resc
                 ${CMAKE_CURRENT_BINARY_DIR}/execute_with_gdb.resc @ONLY)
  configure_file(${STM32F0_TESTS_DIR}/execute.resc
                 ${CMAKE_CURRENT_BINARY_DIR}/execute.resc @ONLY)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.robot
                 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot @ONLY)

  include(RegisterTest)
  register_st(${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot)
endmacro()

add_subdirectory(common)
add_subdirectory(print_hello_world)
add_subdirectory(calculate_floating_point)
add_subdirectory(report_failure_for_wrong_file)
add_subdirectory(return_value)
