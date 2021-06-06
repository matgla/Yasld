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
import struct
import sys
import json

from colorama import Fore, Style, init

from elf_parser import ElfParser
from printers import *

import subprocess

def process_section(elf, section_name, position):
    sections = elf.get_sections()
    if not section_name in sections:
        print_error("Trying to process not existing section: " + section_name)
        print_error("Available sections are: " + str(sections.keys()))
        sys.exit(-1)
    section = sections[section_name]
    if section["address"] != position:
        print_error(section_name + " must be placed at address " + str(position))
        sys.exit(-1)
    return section

def change_visibility_of_wrapped_symbols(symbols, objcopy_executable, elf_filename):
    ind_4 = "        "
    for symbol_name in symbols:
        if symbol_name.endswith("_dl_original"):
            print_debug(ind_4 + "-> " + symbol_name)
            FNULL = open(os.devnull, 'w')
            subprocess.run([objcopy_executable + " -L " + symbol_name \
                            + " " + str(elf_filename)], shell=True, stdout=FNULL, stderr=FNULL)

def process_symbols_visiblity(processed, symbols):
    for name, data in symbols.items():
        if not name or name == "$t" or name == "$d":
            continue

        if data["type"] == "STT_FILE":
                continue

        is_main = name == "main"
        is_global_and_visible = data["binding"] == "STB_GLOBAL" and data["visibility"] != "STV_HIDDEN"
        if not name in processed:
                processed[name] = {"name": name}

        symbol = processed[name]
        if name == "main" or is_global_and_visible:
            if data["section_index"] == "SHN_UNDEF":
                symbol.update({"localization": "external"})
            else:
                symbol.update({"localization": "exported"})

        else:
            symbol.update({"localization": "internal"})

        processed[name].update(symbol)
    return processed

def print_symbols_with_localization(processed_symbols, localization):
    print_debug("      " + localization + " symbols:")
    for name in processed_symbols:
        if processed_symbols[name]["localization"] == localization:
            print_debug("                    " + name)
    print_debug("------------------------------------------------------")

def print_symbols(processed_symbols):
    print_symbols_with_localization(processed_symbols, "external")
    print_symbols_with_localization(processed_symbols, "exported")
    print_symbols_with_localization(processed_symbols, "internal")

def has_relocation(relocations, name):
    for (relocation, offset, index) in relocations:
        if name == relocation:
            return True
    return False

def get_index_of_relocation(relocations, name):
    for (relocation, offset, index) in relocations:
        if name == relocation:
            return index
    return -1


def extract_relocations(relocations, symbols, processed_symbols):
    external_relocations = []
    exported_relocations = []
    local_relocations = []
    index = 0
    for relocation in relocations:
        offset = relocation["offset"]
        if relocation["info_type"] == "R_ARM_THM_CALL":
            continue
        elif relocation["info_type"] == "R_ARM_GOT_BREL":
            symbol_visibility = processed_symbols[relocation["symbol_name"]]["localization"]
            if symbol_visibility == "external":
                lot_index = index
                if not has_relocation(external_relocations, relocation["symbol_name"]):
                    index += 1
                else:
                    lot_index = get_index_of_relocation(external_relocations, relocation["symbol_name"])
                external_relocations.append((relocation["symbol_name"], offset, lot_index))
            elif symbol_visibility == "exported":
                if relocation["symbol_name"] == "main":
                    lot_index = index
                    if not has_relocation(exported_relocations, relocation["symbol_name"]):
                        index += 1
                    else:
                        lot_index = get_index_of_relocation(external_relocations, relocation["symbol_name"])
                    exported_relocations.append((relocation["symbol_name"], offset, lot_index))
            elif symbol_visibility == "internal":
                symbol_value = relocation["symbol_value"]
                section = relocation["section_index"]
                local_relocations.append((symbol_value, offset, index, section, relocation["symbol_name"]))
                index += 1
        elif relocation["info_type"] == "R_ARM_ABS32":
            continue
        elif relocation["info_type"] == "R_ARM_THM_JUMP24":
            continue
        elif relocation["info_type"] == "R_ARM_THM_JUMP11":
            continue
        else:
            print (Fore.RED + "Unknown relocation type. Please fix generate_binary.py: " + relocation["info_type"])
            raise RuntimeError("Script not working for this binary")
    return local_relocations, external_relocations, exported_relocations, index

def extract_data_relocations(relocations, symbols, code_length, lot_offset):
    data_relocations = []
    index = lot_offset
    for relocation in relocations:
        offset = relocation["offset"]
        if relocation["info_type"] == "R_ARM_THM_CALL":
            continue
        elif relocation["info_type"] == "R_ARM_GOT_BREL":
            continue
        elif relocation["info_type"] == "R_ARM_ABS32":
           offset = relocation["offset"]
           symbol_name = relocation["symbol_name"]
           if symbol_name in symbols and \
                (symbols[symbol_name]["type"] == "STT_OBJECT" or \
                 symbols[symbol_name]["type"] == "STT_FUNC"):
                offset = int((offset - code_length)/4)
                symbol_value = relocation["symbol_value"]
                data_relocations.append((symbol_value, offset + lot_offset, relocation["symbol_name"]))
        elif relocation["info_type"] == "R_ARM_THM_JUMP24":
            continue
        elif relocation["info_type"] == "R_ARM_THM_JUMP11":
            continue
        else:
            print (Fore.RED + "Unknown relocation type. Please fix generate_binary.py: " + relocation["info_type"])
            raise RuntimeError("Script not working for this binary")
    return data_relocations

def print_relocations(local_relocations, exported_relocations, external_relocations, data_relocations):
    ind_3 = "      "
    print_debug(ind_3 + "Local relocations:")
    row = [Fore.BLUE + "name", "symbol value", "relocation offset", "relocation index" + Style.RESET_ALL]
    print_debug("{: >25} {: >25} {: >25} {: >22}".format(*row))
    for relocation in local_relocations:
        row = ["(N/A)", hex(relocation[0]), hex(relocation[1]), hex(relocation[2])]
        print_debug("{: >20} {: >20} {: >20} {: >20}".format(*row))

    print_debug(ind_3 + "Exported relocations:")
    row = [Fore.BLUE + "name", "offset", "relocation index" + Style.RESET_ALL]
    print_debug("{: >25} {: >20} {: >25}".format(*row))
    for relocation in exported_relocations:
        row = [relocation[0], hex(relocation[1]), hex(relocation[2])]
        print_debug("{: >20} {: >20} {: >20}".format(*row))

    print_debug(ind_3 + "External relocations:")
    row = [Fore.BLUE + "name", "offset", "relocation index" + Style.RESET_ALL]
    print_debug("{: >25} {: >20} {: >25}".format(*row))
    for relocation in external_relocations:
        row = [relocation[0], hex(relocation[1]), hex(relocation[2])]
        print_debug("{: >20} {: >20} {: >20}".format(*row))

    print_debug(ind_3 + "Data relocations:")
    row = [Fore.BLUE + "name", "symbol value", "relocation offset" + Style.RESET_ALL]
    print_debug("{: >25} {: >25} {: >25}".format(*row))
    for relocation in data_relocations:
        row = [relocation[2], hex(relocation[0]), hex(relocation[1])]
        print_debug("{: >20} {: >20} {: >20}".format(*row))

# def get_lot_relocation_indexes(relocations):
#    relocation_to_index_map =  {}
#    index = 0
#    for relocation in relocations:
#        symbol_name = relocation[0]
#        relocation_to_index_map[symbol_name] = index
#        index += 1
#    return relocation_to_index_map

def add_indexes_to_symbols(processed_symbols):
    symbol_index = 0
    for symbol in processed_symbols:
        symbol_data = processed_symbols[symbol]
        symbol_data["index"] = symbol_index
        processed_symbols[symbol].update(symbol_data)
        symbol_index = symbol_index + 1
    return processed_symbols

def find_symbol_code(api, symbol):
    for sym in api:
        if sym == symbol:
            return api[sym]
    return None

def generate_module(module_name, elf_filename, objcopy_executable, api_file):
    elf = ElfParser(elf_filename)

    code_section = process_section(elf, ".text", 0)
    code_data = bytearray(code_section["data"])

    data_section_address = code_section["address"] + code_section["size"]
    data_section = process_section(elf, ".data", data_section_address)
    data_data = data_section["data"]

    bss_section_address = data_section["address"] + data_section["size"]
    bss_section = process_section(elf, ".bss", bss_section_address)

    ind_3 = "      "
    print_step(ind_3 + "Reading symbols")
    symbols = elf.get_symbols()

    print_step(ind_3 + "Changing visibility of wrapped symbols")

    change_visibility_of_wrapped_symbols(symbols, objcopy_executable, elf_filename)
    symbols = elf.get_symbols()

    processed_symbols = {}
    print_step(ind_3 + "Processing symbols")
    process_symbols_visiblity(processed_symbols, symbols)
    print_symbols(processed_symbols)

    print_step(ind_3 + "Processing relocations")
    relocations = elf.get_relocations()
    local_relocations, external_relocations, exported_relocations, number_of_lot_relocations \
        = extract_relocations(relocations, symbols, processed_symbols)

    #relocation_indexes = get_lot_relocation_indexes(local_relocations + external_relocations)

    print_step(ind_3 + "Creating image of module")

    # +---+---+---+---+
    # | M | S | D | L |
    # +---+---+---+---+
    # |   code size   |
    # +---+---+---+---+
    # |   data size   |
    # +---+---+---+---+
    # |   bss size    |
    # +---+---+---+---+
    # | n.rel | n.sym |
    # +---+---+---+---+
    # | s.siz | l.nam |
    # +---+---+---+---+
    # |     name      |
    # : aligned to 4  :
    # +---+---+---+---+
    # | l.rel. size   |
    # +---+
    # |  relocations  |
    # |   n-rel - 1   |

    # :     n-rel     :
    # +---+---+---+---+
    # |  symbol table |
    # |   s.siz - 1   |
    # :     s.siz     :
    # +---+---+---+---+

    # symbol table
    # +---+---+---+---+
    #

    # pack cookie
    image = bytearray("MSDL", "ascii")
    # pack section size's
    image += struct.pack("<III", len(code_data), len(data_data), len(bss_section["data"]))

    #processed_symbols = add_indexes_to_symbols(processed_symbols)

    symbol_index = 0

    external_symbols = {}
    for symbol in processed_symbols:
        if processed_symbols[symbol]["localization"] == "external":
            if symbols[symbol]["section_index"] == code_section["index"]:
                section = 0
            elif symbols[symbol]["section_index"] == data_section["index"]:
                section = 1
            elif symbols[symbol]["section_index"] == bss_section["index"]:
                section = 1
            else:
                section = 2

            sym = {}
            sym["index"] = symbol_index
            symbol_index += 1
            sym["section"] = section
            sym["size"] = 4 + 2 + 2 + 4
            sym["value"] = symbols[symbol]["value"]
            external_symbols.update({symbol: sym})

    exported_symbols  = {}
    for symbol in processed_symbols:
        if processed_symbols[symbol]["localization"] == "exported":
            if symbols[symbol]["section_index"] == code_section["index"]:
                section = 0
            elif symbols[symbol]["section_index"] == data_section["index"]:
                section = 1
            elif symbols[symbol]["section_index"] == bss_section["index"]:
                section = 1
            else:
                section = 2

            sym = {}
            sym["index"] = symbol_index
            symbol_index += 1
            sym["section"] = section
            sym["size"] = 4 + 2 + 2 + 4
            sym["value"] = symbols[symbol]["value"]
            exported_symbols.update({symbol: sym})

    data_relocations = extract_data_relocations(\
        relocations, symbols, len(code_data), \
        number_of_lot_relocations)
    print_relocations(local_relocations, exported_relocations, external_relocations, data_relocations)
    print_step(ind_3 + "Patch offsets")
    for relocation in local_relocations + external_relocations + exported_relocations:
        offset = relocation[1]
        old = struct.unpack_from("<I", code_data, offset)[0]
        new = relocation[2] * 4
        struct.pack_into("<I", code_data, offset, new)

    index = number_of_lot_relocations

    local_relocations_to_image = []
    for relocation in local_relocations:
        symbol_value, offset, index, section, name = relocation
        rel = {}
        rel["lot_index"] = index
        rel["symbol_value"] = symbol_value
        rel["section"] = section
        rel["symbol_name"] = name
        local_relocations_to_image.append(rel)

    processed = []
    external_relocations_to_image = []
    for relocation in  external_relocations:
        symbol, value, index = relocation
        if symbol in processed:
            continue
        rel = {}
        rel["lot_index"] = index
        rel["symbol_index"] = external_symbols[symbol]["index"]
        rel["symbol_name"] = symbol
        external_relocations_to_image.append(rel)
        processed.append(symbol)

    processed = []
    exported_relocations_to_image = []
    for relocation in exported_relocations:
        symbol, value, index = relocation
        if symbol in processed:
            continue
        rel = {}
        rel["lot_index"] = index
        rel["symbol_index"] = exported_symbols[symbol]["index"]
        exported_relocations_to_image.append(rel)
        processed.append(symbol)

    data_relocations_to_image = []
    for relocation in data_relocations:
        symbol_value, offset, name = relocation
        rel = {}
        rel["offset"] = offset
        rel["symbol"] = symbol_value
        rel["symbol_name"] = name
        data_relocations_to_image.append(rel)

    total_relocations = len(data_relocations_to_image) \
        + len(local_relocations_to_image) + len(external_relocations_to_image) \
        + len(exported_relocations_to_image)

    # pack number of relocations
    image += struct.pack("<HHH", \
                         len(external_relocations_to_image), \
        len(local_relocations_to_image), len(data_relocations_to_image))

    image += struct.pack("<H", len(external_symbols))

    symbol_to_image = []

    print_step("Relocation table placed at: " + hex(len(image)))
    row = ["symbol", "index", "offset_to_symbol"]
    print_step("External relocations")
    print_step("{: >50} {: >10} {: >10}".format(*row))
    relocation_position = 0

    # for rel in exported_relocations_to_image:
    #     sizeof_relocation = 8

    #     offset_to_symbol_table = (total_relocations - relocation_position) * sizeof_relocation
    #     symbol_name = rel["symbol_name"]
    #     symbol_offset = 0
    #     for symbol in exported_symbols:
    #         if symbol == symbol_name:
    #             break
    #         symbol_offset += 12
    #     offset_to_symbol = offset_to_symbol_table + symbol_offset
    #     row = [symbol_name, rel["lot_index"], offset_to_symbol]
    #     print_step("{: >50} {: >10} {: >10}".format(*row))
    #     image += struct.pack("<II", rel["lot_index"], offset_to_symbol)
    #     relocation_position += 1

    exported_symbols_size = 0
    for symbol in exported_symbols:
        exported_symbols_size += 12

    row = ["symbol", "index", "offset_to_symbol"]
    print_step("Exported relocations")
    print_step("{: >50} {: >10} {: >10}".format(*row))

    for rel in external_relocations_to_image:
        sizeof_relocation = 8

        offset_to_symbol_table = (total_relocations - relocation_position) * sizeof_relocation
        symbol_name = rel["symbol_name"]
        symbol_offset = exported_symbols_size
        for symbol in external_symbols:
            if symbol == symbol_name:
                break
            symbol_offset += external_symbols[symbol]["size"]
        offset_to_symbol = offset_to_symbol_table + symbol_offset
        row = [symbol_name, rel["lot_index"], hex(offset_to_symbol)]
        print_step("{: >50} {: >10} {: >10}".format(*row))
        image += struct.pack("<II", rel["lot_index"], offset_to_symbol)
        relocation_position += 1

    row = ["name", "index", "section", "offset_to_symbol"]
    print_step("Local relocations")
    print_step("{: >40} {: >10} {: >10} {: >10}".format(*row))

    for rel in local_relocations_to_image:
        value = rel["symbol_value"]
        if rel["section"] == code_section["index"]:
            section = 0

        elif rel["section"] == data_section["index"] or rel["section"] == bss_section["index"]:
            section = 1
            value = value - len(code_data)
        else:
            raise "Unsupported section for local relocation"

        lot_index_and_section = (rel["lot_index"] << 1) | section
        row = [rel["symbol_name"], rel["lot_index"], section, hex(value)]
        print_step("{: >40} {: >10} {: >10} {: >10}".format(*row))

        image += struct.pack("<II", lot_index_and_section, value)
    row = ["name", "offset", "offset_to_symbol"]
    print_step("Data relocations")
    print_step("{: >50} {: >10} {: >10} ".format(*row))

    for rel in data_relocations_to_image:
        row = [rel["symbol_name"], hex(rel["offset"]), hex(rel["symbol"])]
        print_step("{: >50} {: >10} {: >10}".format(*row))
        image += struct.pack("<II", rel["offset"], rel["symbol"])

    print_step("Symbol section placed at: " + hex(len(image)));

    print_step("Symbol Table:")
    row = ["name", "size", "visibility", "section", "value"]
    print_step("{: >50} {: >10} {: >10} {: >10} {: >15}".format(*row))


    i = 0
    api = {}
    with open(api_file) as codes_json:
        api = json.loads(codes_json.read())

    for sym in exported_symbols:
        symbol = exported_symbols[sym]
        value = symbol["value"]
        if symbol["section"] == 1:
            print_debug("This is data relocation, calculating offset")
            value -= len(code_data)

        if sym == "main":
            symbol_code = find_symbol_code(api, sym)
        else:
            symbol_code = find_symbol_code(api, "undefined")

        if symbol_code == None:
            raise RuntimeError("Can't find symbol in api file: " + sym)

        image += struct.pack("<HHII", symbol["size"], symbol["section"], value, symbol_code)

    for sym in external_symbols:
        symbol = external_symbols[sym]
        value = symbol["value"]
        if symbol["section"] == 1:
            print_debug("This is data relocation, calculating offset")
            value -= len(code_data)
        symbol_code = find_symbol_code(api, sym)
        if symbol_code == None:
            raise RuntimeError("Can't find symbol in api file: " + sym)
        image += struct.pack("<HHII", symbol["size"], symbol["section"], value, symbol_code)

    if (len(image) % 16):
        image += bytearray('\0' * (16 - (len(image) % 16)), "ascii")

    image += code_data
    image += data_data
    with open(module_name + ".bin", "wb") as file:
        file.write(image)

parser = argparse.ArgumentParser(description = "Relocable modules and shared libraries generator")
parser.add_argument("-o", "--output", dest="output_directory", action="store", help="Path to output file")
parser.add_argument("--module_name", dest="module_name", action="store", help="Module name")
parser.add_argument("-i", "--elf_filename", dest="elf_filename", action="store", help="Path to module ELF file")
parser.add_argument("--objcopy", dest="objcopy_executable", action="store", help="Path to objcopy executable")
parser.add_argument("--as_executable", dest="as_executable", action="store_true", help="Generate module as executable")
parser.add_argument("-v", "--verbose", dest="enable_debugs", action="store_true", help="Additional logs")
parser.add_argument("--disable_logs", dest="disable_logs", action="store_true", help="Disable logs")
parser.add_argument("--api", dest="api_file", action="store", help="API description file", required="true")
args, rest = parser.parse_known_args()

def configure_logs(args):
    if args.enable_debugs == True:
        enable_debugs()

    if args.disable_logs == True:
        disable_prints()

def display_header():
    print_menu("==========================================")
    print_menu("=            MODULE_GENERATOR            =")
    print_menu("==========================================")

def verify_arguments():
    if not args.elf_filename:
        print_error("Please provide module ELF file (--elf_filename=<path_to_elf_file>)")
        sys.exit(-1)

def main():
    init()
    configure_logs(args)
    display_header()

    print_step("STEP 1. Initialization")
    verify_arguments()

    print_step("STEP 2. Generating module: " + args.module_name + ", ELF: " + args.elf_filename)

    generate_module(args.module_name, args.elf_filename, args.objcopy_executable, args.api_file)

main()
