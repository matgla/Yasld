#
# AddTestWithModule.cmake
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

macro(add_test_with_module)
  set(prefix TEST)
  set(optionArgs "")
  set(singleValueArgs
      NAME
      MODULE
      SCRIPTS
      ROBOT_COMMON_FILE
      BOARD_FILE)
  set(multiValueArgs SOURCES LIBRARIES)

  include(CMakeParseArguments)

  cmake_parse_arguments(
    ${prefix}
    "${optionArgs}"
    "${singleValueArgs}"
    "${multiValueArgs}"
    ${ARGN})

  if(NOT DEFINED TEST_NAME)
    message(FATAL_ERROR "NAME must be defined")
  endif()

  if(NOT DEFINED TEST_SOURCES)
    message(FATAL_ERROR "SOURCES must be defined")
  endif()

  if(NOT DEFINED TEST_ROBOT_COMMON_FILE)
    message(FATAL_ERROR "ROBOT_COMMON_FILE must be defined")
  endif()

  add_executable(${TEST_NAME})

  target_sources(${TEST_NAME} PRIVATE ${TEST_SOURCES})

  target_link_libraries(${TEST_NAME} PRIVATE ${TEST_LIBRARIES} yasld yasld_arch)

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
  get_filename_component(MODULE_PATH ${MODULE_PATH} REALPATH)

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
    DEPENDS ${TEST_NAME} modules)

  set(renode_test_binary ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_test.bin)
  set(renode_board_file ${TEST_BOARD_FILE})

  configure_file(${TEST_SCRIPTS}/execute.resc
                 ${CMAKE_CURRENT_BINARY_DIR}/execute.resc)

  configure_file(${TEST_SCRIPTS}/execute_gdb.resc
                 ${CMAKE_CURRENT_BINARY_DIR}/execute_gdb.resc)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.robot
                 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot @ONLY)

  configure_file(${TEST_SCRIPTS}/${TEST_ROBOT_COMMON_FILE}
                 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_ROBOT_COMMON_FILE} @ONLY)

  include(RegisterTest)

  register_st(${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot)
endmacro()
