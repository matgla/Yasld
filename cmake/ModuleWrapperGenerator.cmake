# This file is part of MSOS project. This is simple OS for embedded development devices.
# Copyright (C) 2019 Mateusz Stadnik
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

function (configure_virtual_env)
    if (NOT TARGET configure-venv)
        find_program (virtualenv_exec virtualenv)

        if(NOT virtualenv_exec)
            message(FATAL_ERROR "Couldn't find virtualenv")
        endif()

        add_custom_command(
            OUTPUT venv.stamp
            COMMAND ${virtualenv_exec} ${PROJECT_BINARY_DIR}/module_generator_env
            COMMAND ${PROJECT_BINARY_DIR}/module_generator_env/bin/pip install -r ${PROJECT_SOURCE_DIR}/scripts/requirements.txt --upgrade
            DEPENDS ${PROJECT_SOURCE_DIR}/scripts/requirements.txt
        )

        add_custom_target(
            configure-venv
            DEPENDS venv.stamp
        )
    endif()

endfunction()

function (add_module module_name module_library)
    if (${arch} STREQUAL "ARM")
        configure_virtual_env()

        find_file (GENERATE_BINARY generate_binary.py ${PROJECT_SOURCE_DIR}/scripts)

        add_dependencies(${module_name} api_generator)

        add_custom_command(
            TARGET ${module_name}
            POST_BUILD
            COMMAND echo "${PROJECT_BINARY_DIR}/module_generator_env/bin/python3 ${GENERATE_BINARY} generate_wrapper_code --disable_logs --elf_filename=$<TARGET_FILE:${module_name}> --module_name=${module_name} --objcopy=${CMAKE_OBJCOPY} --as_executable --api=${PROJECT_SOURCE_DIR}/api/symbol_codes.json"
            COMMAND ${PROJECT_BINARY_DIR}/module_generator_env/bin/python3 ${GENERATE_BINARY}
            generate_wrapper_code --elf_filename=$<TARGET_FILE:${module_name}> --module_name=${module_name}
            --objcopy=${CMAKE_OBJCOPY} --as_executable --api=${generated_api_file}
            COMMAND echo "Module generated: ${module_name}"
            DEPENDS ${GENERATE_BINARY} ${module_name} venv.stamp api_generator
            VERBATIM
        )

        add_dependencies(${module_name} configure-venv)
    elseif (${arch} STREQUAL "x86")
    endif()
endfunction ()

function (add_module_flags_target)
    if (TARGET module_flags)
        return()
    endif ()

    add_library(module_flags INTERFACE)

    if (${arch} STREQUAL "ARM")
        target_link_options(module_flags INTERFACE
            "${hal_linker_flags};-T${PROJECT_SOURCE_DIR}/linker_scripts/dynamic_module.ld;-nostartfiles;-nodefaultlibs;-nostdlib;-Wl,--unresolved-symbols=ignore-in-object-files;-Wl,--emit-relocs;-fvisibility=hidden;")

        set(module_common_flags "-fvisibility-inlines-hidden;-fno-function-sections;-fno-data-sections;-fno-section-anchors;-msingle-pic-base;-mno-pic-data-is-text-relative;-fPIE;-mlong-calls;-fvisibility=hidden;")
        target_compile_options(module_flags INTERFACE
            $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a;${hal_cxx_compilation_flags};${module_common_flags}>
            $<$<COMPILE_LANGUAGE:C>:${hal_c_compilation_flags};${module_common_flags}>

            $<$<CONFIG:DEBUG>:-Og -g>
            $<$<CONFIG:RELEASE>:-Os>)
        target_include_directories(module_flags INTERFACE ${PROJECT_SOURCE_DIR}/include)
    else ()
        target_link_options(module_flags INTERFACE
            "${hal_linker_flags}")


        target_compile_options(module_flags
            INTERFACE
                $<$<COMPILE_LANGUAGE:CXX>:-std=c++2a;-fPIC>
                $<$<CONFIG:DEBUG>:-Og -g>
                $<$<CONFIG:RELEASE>:-Os>
    )
    endif ()
endfunction()
