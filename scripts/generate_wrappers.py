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

import os
import argparse

from elftools.elf.elffile import ELFFile
from elftools.elf.sections import SymbolTableSection
from elftools.elf.relocation import RelocationSection
from elftools.elf.descriptions import describe_reloc_type
from jinja2 import Template, Environment, FileSystemLoader
from colorama import Fore, Style, init
from pathlib import Path


def get_symbols(elf_filename):
    symbols = {}
    with open(elf_filename, "rb") as elf_file:
        elf = ELFFile(elf_file)
        for section in elf.iter_sections():
            if not isinstance(section, SymbolTableSection):
                continue
            for symbol in section.iter_symbols():
                data = {}
                data["type"] = symbol["st_info"]["type"]
                data["binding"] = symbol["st_info"]["bind"]
                data["name"] = symbol["st_name"]
                data["value"] = symbol["st_value"]
                data["size"] = symbol["st_size"]
                data["section_index"] = symbol["st_shndx"]
                data["visibility"] = symbol["st_other"]["visibility"]
                symbols[symbol.name] = data
    return symbols

def get_public_functions_from_symbols(symbols):
    filtered_symbols = {}
    for symbol_key in symbols:
        symbol = symbols[symbol_key]
        if symbol["type"] == "STT_FUNC":
            if symbol["visibility"] != "STV_HIDDEN" and (symbol["binding"] == "STB_GLOBAL" or symbol["binding"] == "STB_WEAK"):
                filtered_symbols[symbol_key] = symbol
            if symbol_key == "main" and (symbol["binding"] == "STB_GLOBAL" or symbol["binding"] == "STB_WEAK"):
                filtered_symbols[symbol_key] = symbol
        if symbol_key.endswith("_dl_original"):
            filtered_symbols[symbol_key] = symbol
    return filtered_symbols

def get_relocations(elf_filename):
    relocations = []
    with open(elf_filename, "rb") as elf_file:
        elf = ELFFile(elf_file)
        for section in elf.iter_sections():
            if not isinstance(section, RelocationSection):
                continue
            symbols = elf.get_section(section["sh_link"])
            for relocation in section.iter_relocations():
                if relocation["r_info_sym"] == 0:
                    continue
                relocation_data = {}
                relocation_data["offset"] = relocation["r_offset"]
                relocation_data["info"] = relocation["r_info"]
                relocation_data["info_type"] = describe_reloc_type(relocation["r_info_type"], elf)
                relocation_data["symbol"] = relocation["r_info_sym"]
                symbol = symbols.get_symbol(relocation_data["symbol"])
                if symbol["st_name"] == 0:
                    symbol_section = elf.get_section(symbol["st_shndx"])
                    relocation_data["symbol_name"] = symbol_section.name
                else:
                    relocation_data["symbol_name"] = symbol.name

                relocation_data["symbol_value"] = symbol["st_value"]

                relocations.append(relocation_data)
    return relocations

def get_sections(elf_filename):
    sections = {}
    with open(elf_filename, "rb") as elf_file:
        elf = ELFFile(elf_filename)
        for section in elf.iter_sections():
            section_data = {}
            section_data["name"] = section.name
            section_data["address"] = section["sh_addr"]
            section_data["type"] = section["sh_type"]
            section_data["flags"] = section["sh_flags"]
            section_data["offset"] = section["sh_offset"]
            section_data["size"] = section["sh_size"]
            section_data["link"] = section["sh_link"]
            section_data["info"] = section["sh_info"]
            section_data["addralign"] = section["sh_addralign"]
            section_data["size"] = section["sh_entsize"]
            section_data["data"] = section.data()
            sections[section.name] = section_data

    return sections

def get_symbol_names(symbols):
    symbol_names = []
    for key in symbols:
        symbol_names.append(key)
    return symbol_names

import subprocess

def wrap_symbols(symbol_names, filename, objcopy_executable):
    symbols_to_generate = {}
    for symbol in symbol_names:
        # print ("Processing: ", symbol)
        symbol_name = symbol

        if (symbol_name.endswith("_dl_original")):
            symbol_name = symbol_name.replace("_dl_original", "")

        wrapped_symbol = symbol_name + "_dl_original"
        # print (Fore.YELLOW + "[INF]" + Style.RESET_ALL + "          Renaming " + symbol + " -> " + wrapped_symbol)

        symbols_to_generate.update({symbol_name: wrapped_symbol})
        if symbol.endswith("_dl_original"):
            continue
        command = objcopy_executable + " --redefine-sym " + symbol + "=" + symbol + "_dl_original " + str(filename)
        # print ("Execute: ", command)
        subprocess.run([command], shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    return symbols_to_generate

def generate_wrapper_file(symbols_to_generate, output_directory):
    output_filename = output_directory + "/wrapped_symbols.s"
    # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + "          Generating function calls wrappers: ", output_filename)
    if not os.path.exists(output_directory):
        # print (Fore.YELLOW + "[INF]" + Style.RESET_ALL + "      Creating directory for output")
        os.makedirs(output_directory)

    current_path = os.path.dirname(os.path.abspath(__file__))
    template_loader = FileSystemLoader(current_path)
    env = Environment(loader = template_loader)
    t = env.get_template("wrapped_symbols.s.template")
    # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + "          Generated wrapper for public symbols")

    with open(output_filename, "w+") as file:
        file.write(t.render(wrapped_symbols = symbols_to_generate))


def main():
    init()

    parser = argparse.ArgumentParser(description = "Relocable modules and shared libraries generator")
    parser.add_argument("-i", "--input", dest="input_directory", action="store", help="Path to input file")
    parser.add_argument("-o", "--output", dest="output_directory", action="store", help="Path to output file")
    parser.add_argument("--objcopy", dest="objcopy_executable", action="store", help="Path to objcopy executable")
    parser.add_argument("--ar", dest="ar", action="store", help="Path to ar executable")
    parser.add_argument("--print_dependencies", dest="print_dependencies", action="store_true", help="Prints only generated files")
    parser.add_argument("--module_name", dest="module_name", action="store", help="Module name")
    parser.add_argument("--elf_filename", dest="elf_filename", action="store", help="Path to module ELF file")

    args, rest = parser.parse_known_args()

    symbols_to_generate = {}

    # print(Fore.CYAN + "==========================================")
    # print(Fore.CYAN + "=           WRAPPERS_GENERATOR           =")
    # print(Fore.CYAN + "==========================================")
    # print(Style.RESET_ALL)

    # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + " STEP 1. Initialization")

    # if not args.input_directory:
    #     print(Fore.RED + "[ERR] Please provide directory with .o files (--input_directory=<path_to_directory>)")
    #     return

    # if not args.objcopy_executable:
    #     print(Fore.RED + "[ERR] Please objcopy executable (--objcopy=<path_to_objcopy>)")
    #     return

    files = list(Path(args.input_directory).rglob("*.o"))

    # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + " STEP 2. Wrapping symbols in files")
    file_number = 1
    for file in files:
        if (str(file).endswith("wrapped_symbols.s.o")):
            continue
        # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + "   [" + str(file_number) + "/" + str(len(files)) + "] " + str(file))
        symbols = get_symbols(file)

        public_symbols = get_public_functions_from_symbols(symbols)
        symbols_to_rename = get_symbol_names(public_symbols)
        symbols_to_generate.update(wrap_symbols(symbols_to_rename, file, args.objcopy_executable))

    # print(Fore.YELLOW + "[INF]" + Style.RESET_ALL + " STEP 3. Generating wrapper file")

    generate_wrapper_file(symbols_to_generate, args.output_directory)
    output_filename = args.output_directory + "/lib" + args.module_name + "_wrapped.a"

    files = list(Path(args.output_directory).rglob("*.o"))
    to_lib = ""
    for file in files:
        to_lib += str(file) + " "
    command = args.ar + " -crs " + output_filename + " " + to_lib
    subprocess.run([command], shell=True, stdout=subprocess.PIPE)

    # print(Fore.GREEN + "=========================================" + Style.RESET_ALL)

    # print(Fore.GREEN + "[SUCCESS]" + "All steps succeeded" + Style.RESET_ALL)
    # print()


main()
