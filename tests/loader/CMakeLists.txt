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

add_executable(msos_test_loader)

target_sources(msos_test_loader 
    PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

target_link_libraries(msos_test_loader 
    PRIVATE 
        msos_dynamic_linker
)

target_compile_definitions(msos_test_loader 
    PRIVATE 
        -DLIB_PATH=\"$<TARGET_FILE:module_a>\"
)

target_compile_options(msos_test_loader PRIVATE -fPIE)

target_link_options(msos_test_loader PRIVATE -rdynamic)
