#
# RegisterTest.cmake
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

find_package(CMakeUtils REQUIRED)

message(STATUS "${CMAKE_MODULE_PATH}")
include(virtualenv)

create_virtualenv(
  st
  ${CMAKE_CURRENT_LIST_DIR}/../scripts/requirements.txt
  ${PROJECT_BINARY_DIR}/venvs)

set(register_script ${CMAKE_CURRENT_LIST_DIR}/../scripts/register_test.py)

function(register_st file)
  if(NOT
     DEFINED
     ST_TESTS_FILE)
    message(FATAL_ERROR "YAML file with tests must be defined")
  endif()

  set(command
      ${st_python_executable}
      ${register_script}
      -f
      ${ST_TESTS_FILE}
      -i
      ${file})

  message(STATUS "Registering test: ${file}")
  message(STATUS "Command: ${command}")

  execute_process(COMMAND ${command} COMMAND_ERROR_IS_FATAL ANY)
endfunction()
