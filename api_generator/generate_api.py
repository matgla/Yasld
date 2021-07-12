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

#!/usr/bin/python3

import argparse
import json
import os

import clang
from clang.cindex import CursorKind

from pathlib import Path

def generate_header(output):
    with open(output, "w") as file:
        file.write("/*************************************/\n")
        file.write("/*    FILE GENERATED AUTOMATICALLY   */\n")
        file.write("/*************************************/\n\n")
        file.write("// This file is part of MSOS project.\n")
        file.write("// Copyright (C) 2020 Mateusz Stadnik\n")
        file.write("//\n")
        file.write("// This program is free software: you can redistribute it and/or modify\n")
        file.write("// it under the terms of the GNU General Public License as published by\n")
        file.write("// the Free Software Foundation, either version 3 of the License, or\n")
        file.write("// (at your option) any later version.\n")
        file.write("//\n")
        file.write("// This program is distributed in the hope that it will be useful,\n")
        file.write("// but WITHOUT ANY WARRANTY; without even the implied warranty of\n")
        file.write("// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n")
        file.write("// GNU General Public License for more details.\n")
        file.write("//\n")
        file.write("// You should have received a copy of the GNU General Public License\n")
        file.write("// along with this program.  If not, see <https://www.gnu.org/licenses/>.\n\n")

        file.write("#pragma once\n\n")

def fully_qualified(c):
    if c is None:
        return ''
    elif c.kind == CursorKind.TRANSLATION_UNIT:
        return ''
    else:
        res = fully_qualified(c.semantic_parent)
        if res != '':
            return res + '::' + c.spelling
    return c.spelling


def generate_file(output, input, config, name, generated_symbols):
    output_file = output + "/" + name + "_symbol_codes.h"
    generate_header(output_file)

    index_start = config["range_start"]
    index_max = config["range_stop"]
    index = index_start
    with open(output_file, "w") as file:
        file.write("enum " + name + "_SymbolCode\n")
        file.write("{\n")

        for include in config["includes"]:
            idx = clang.cindex.Index.create()
            filepath = None
            for path in Path(input).rglob(include):
                filepath = path
                break
            if filepath is None:
                raise Exception("File not found: " + include + " in: " + input)
            tu = idx.parse(filepath, args='-xc++ --std=c++2a'.split())
            for c in tu.cursor.walk_preorder():
                if c.kind == CursorKind.FUNCTION_DECL or c.kind == CursorKind.VAR_DECL:
                    if os.path.abspath(filepath) != os.path.abspath(c.location.file.name):
                        continue
                    if index >= index_max - 1:
                        raise "Range is too small, please increase index range"
                    symbol_name = c.referenced.spelling
                    file.write("    " + name + "_" + symbol_name + " = " + str(index) + ",\n")
                    generated_symbols[symbol_name] = index
                    index = index + 1

        file.write("};\n\n")
        file.write("#define add_" + name + "_symbol_codes() \\" + "\n")
        for c in tu.cursor.walk_preorder():
            if c.kind == CursorKind.FUNCTION_DECL:
                if os.path.abspath(filepath) == os.path.abspath(c.location.file.name):
                    symbol_name = c.referenced.spelling
                    return_type = c.referenced.result_type.spelling
                    file.write("    msos::dl::SymbolAddress{" + name + "_SymbolCode::" + name + "_" + symbol_name + ", address_resolver<" + return_type + "(");
                    args_num = 0

                    for arg in c.referenced.get_arguments():
                        if arg.kind == CursorKind.PARM_DECL:
                            args_num = args_num + 1
                    i = 0
                    for arg in c.referenced.get_arguments():
                        if arg.kind == CursorKind.PARM_DECL:
                            file.write(arg.type.spelling)
                            if i < args_num - 1:
                                file.write(", ")
                            i = i + 1
                    # if c.referenced.type.is_function_variadic():
                    #     if i != 0:
                    #         file.write(", ")
                    #     file.write("...")


                    file.write(")>::get(&" + symbol_name + ")}, \\" + "\n")
            if c.kind == CursorKind.VAR_DECL:
                if os.path.abspath(filepath) == os.path.abspath(c.location.file.name):
                    symbol_name = c.referenced.spelling
                    file.write("    msos::dl::SymbolAddress{" + name + "_SymbolCode::" + name + "_" + symbol_name + ", &" + symbol_name + "}, \\" + "\n");
        file.write("\n\n")



def generate_api(api_file, output, input):
    generate_header(output + "/symbol_codes.h")
    generated_symbols = {}

    with open(api_file) as codes_json:
        api = json.loads(codes_json.read())

        with open(output + "/symbol_codes.h", "a") as file:
            file.write("enum SymbolCode\n")
            file.write("{\n")
            for lib in api:
                if lib == "generate":
                    continue
                for sym in api[lib]:
                    file.write("    " + lib + "_" + sym + " = " + str(api[lib][sym]) + ",\n")
                    generated_symbols[sym] = api[lib][sym]
            file.write("};\n\n")

        for lib in api:
            if lib == "generate":
                for lib_to_generate in api[lib]:
                    generate_file(output, input, api[lib][lib_to_generate], lib_to_generate, generated_symbols)
    with open(output + "/generated_symbols.json", "w") as out:
        json.dump(generated_symbols, out)

parser = argparse.ArgumentParser(description = "Relocable modules and shared libraries generator")
parser.add_argument("--api", dest="api_file", action="store", help="API descryption file", required="true")
parser.add_argument("--output", "-o", dest="output", action="store", help="Generated file destination", required="true")
parser.add_argument("--input", "-i", dest="input", action="store", help="Input working dir", required="true")
args, rest = parser.parse_known_args()

generate_api(args.api_file, args.output, args.input)
