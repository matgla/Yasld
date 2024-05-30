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

set(CURRENT_DIR ${CMAKE_CURRENT_LIST_DIR})

if (${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Darwin")
  set(renode_executable mono /Applications/Renode.app/Contents/MacOS/bin/Renode.exe)
else () 
  find_program(renode_executable renode)
endif ()

function(import_module TEST_MODULE)
  get_filename_component(module_name ${TEST_MODULE} NAME)
  add_executable(${module_name}.yaff IMPORTED)

  set_property(TARGET ${module_name}.yaff PROPERTY IMPORTED_LOCATION
                                                   ${TEST_MODULE})
  set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS ${TEST_MODULE})
endfunction()

macro(add_test_with_module)
  set(prefix TEST)
  set(optionArgs "")
  set(singleValueArgs
      NAME
      MODULE
      SCRIPTS
      ROBOT_COMMON_FILE
      BOARD_FILE)
  set(multiValueArgs SOURCES LIBRARIES LAYOUT)

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

  if(NOT DEFINED TEST_LAYOUT)
    add_custom_command(
      TARGET ${TEST_NAME}
      POST_BUILD
      COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TEST_NAME}>
      COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TEST_NAME}>
              ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin)
    set(TEST_LAYOUT "${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin:0x10000"
                    "${STM32F0_TEST_MODULES_DIR}/${TEST_MODULE}.yaff:0x10000")
  else()
    add_custom_command(
      TARGET ${TEST_NAME}
      POST_BUILD
      COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TEST_NAME}>
      COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TEST_NAME}>
              ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.bin)
  endif()

  add_dependencies(${TEST_NAME} modules)

  set(is_first TRUE)
  foreach(TEST_MODULE ${TEST_LAYOUT})
    if(NOT is_first)
      string(FIND ${TEST_MODULE} ":" delimiter)
      string(
        SUBSTRING ${TEST_MODULE}
                  0
                  ${delimiter}
                  TEST_MODULE)
      import_module(${TEST_MODULE})
    endif()
    set(is_first FALSE)
  endforeach()

  include(virtualenv)
  create_virtualenv(test_builder ${CURRENT_DIR}/requirements.txt
                    ${PROJECT_BINARY_DIR}/venvs)

  add_custom_target(
    ${TEST_NAME}_image ALL
    COMMAND ${test_builder_python_executable} ${CURRENT_DIR}/build_binary.py -i
            ${TEST_LAYOUT} -o ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_test.bin
    DEPENDS ${TEST_NAME} modules)

  set(renode_test_binary ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}_test.bin)
  set(renode_board_file ${TEST_BOARD_FILE})

  configure_file(${TEST_SCRIPTS}/execute.resc
                 ${CMAKE_CURRENT_BINARY_DIR}/execute.resc)

  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${TEST_NAME}.robot
                 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot @ONLY)

  configure_file(${TEST_SCRIPTS}/${TEST_ROBOT_COMMON_FILE}
                 ${CMAKE_CURRENT_BINARY_DIR}/${TEST_ROBOT_COMMON_FILE} @ONLY)

  include(RegisterTest)

  register_st(${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}.robot)

  add_custom_target(${TEST_NAME}_run
    COMMAND ${renode_executable} --console --disable-xwt ${CMAKE_CURRENT_BINARY_DIR}/execute.resc
    DEPENDS ${TEST_NAME}_image 
    VERBATIM
    USES_TERMINAL
  )
endmacro()
